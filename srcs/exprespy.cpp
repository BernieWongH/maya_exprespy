#include <Python.h>

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MPxNode.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MDataBlock.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>

#ifdef _WIN32
#pragma warning(disable: 4127)
#endif

#define EXPRESPY_NODE_ID  0x00070004  // ���������ƏՓ˂���Ȃ珑�������邱�ƁB(Free: 0x00000000 �` 0x0007ffff)


//=============================================================================
/// Functions.
//=============================================================================
/// ���W���[�����C���|�[�g����BPyImport_ImportModule ��荂�����B
static inline PyObject* _importModule(const char* name)
{
    PyObject* nameo = PyString_FromString(name);
    if (nameo) {
        PyObject* mod = PyImport_Import(nameo);
        Py_DECREF(nameo);
        return mod;
    }
    return NULL;
}

/// �L�[�ƒl�̎Q�Ƃ�D���� dict �ɒl���Z�b�g����B
static inline void _setDictStealValue(PyObject* dic, PyObject* keyo, PyObject* valo)
{
    if (valo) {
        PyDict_SetItem(dic, keyo, valo);
        Py_DECREF(valo);
    } else {
        PyDict_SetItem(dic, keyo, Py_None);
    }
    Py_DECREF(keyo);
}

/// dict �� int ���L�[�Ƃ��� PyObject* ���Z�b�g����B
static inline void _setDictValue(PyObject* dic, int key, PyObject* valo)
{
    PyObject* keyo = PyInt_FromLong(key);
    if (keyo) {
        PyDict_SetItem(dic, keyo, valo);
        Py_DECREF(keyo);
    }
}

/// dict ���� int ���L�[�Ƃ��� PyObject* �𓾂�B
static inline PyObject* _getDictValue(PyObject* dic, int key)
{
    PyObject* keyo = PyInt_FromLong(key);
    if (keyo) {
        PyObject* valo = PyDict_GetItem(dic, keyo);
        Py_DECREF(keyo);
        return valo;
    }
    return NULL;
}

/// �^���ʍς݂� PyObject* �̒l�� double3 �� data �ɓ���B
static inline void _getDouble3ValueFromPyObject(MFnNumericData& fn, PyObject* valo)
{
    PyObject* xo = PyObject_GetAttrString(valo, "x");  // DECREF �s�v�B
    PyObject* yo = PyObject_GetAttrString(valo, "y");
    PyObject* zo = PyObject_GetAttrString(valo, "z");
    fn.setData(
        xo ? PyFloat_AS_DOUBLE(xo) : 0.,
        yo ? PyFloat_AS_DOUBLE(yo) : 0.,
        zo ? PyFloat_AS_DOUBLE(zo) : 0.
    );
}

/// �^���ʍς݂� PyObject* �̒l�� MMatrix �ɓ���B
static inline void _getMatrixValueFromPyObject(MMatrix& mat, PyObject* valo)
{
    PyObject* getElement = PyObject_GetAttrString(valo, "getElement");  // DECREF �s�v�B
    if (getElement) {
        PyObject* idxs[] = {
            PyInt_FromLong(0), PyInt_FromLong(1), PyInt_FromLong(2), PyInt_FromLong(3)
        };
        PyObject* args = PyTuple_New(2);
        for (unsigned i=0; i<4; ++i) {
            PyTuple_SET_ITEM(args, 0, idxs[i]);
            for (unsigned j=0; j<4; ++j) {
                PyTuple_SET_ITEM(args, 1, idxs[j]);

                //PyObject* args = Py_BuildValue("(ii)", i, j);
                PyObject* vo = PyObject_CallObject(getElement, args);
                //Py_DECREF(args);
                if (vo) {
                    mat.matrix[i][j] = PyFloat_AS_DOUBLE(vo);
                    Py_DECREF(vo);
                } else {
                    mat.matrix[i][j] = MMatrix::identity[i][j];
                }
            }
        }

        PyTuple_SET_ITEM(args, 0, idxs[0]);
        PyTuple_SET_ITEM(args, 1, idxs[1]);
        Py_DECREF(args);
        Py_DECREF(idxs[2]);
        Py_DECREF(idxs[3]);

    } else {
        mat.setToIdentity();
    }
}


//=============================================================================
/// Exprespy node class.
//=============================================================================
class Exprespy : public MPxNode {
    PyObject* _globals;
    PyObject* _base_locals;
    PyObject* _input;
    PyObject* _output;
    PyObject* _type_Vector3;
    PyObject* _type_Vector4;
    PyObject* _type_EulerRot;
    PyObject* _type_Matrix44;
    PyObject* _locals;
    PyCodeObject* _codeobj;

    MStatus _compileCode(MDataBlock&);
    MStatus _executeCode(MDataBlock&);

    void _setInputScalar(int key, const double& val);
    void _setInputString(int key, const MString& str);
    void _setInputDouble3(int key, MObject& data);
    void _setInputMatrix(int key, MObject& data);

public:
    static MTypeId id;
    static MString name;

    static MObject aCode;
    static MObject aCompiled;
    static MObject aInput;
    static MObject aOutput;

    static void* creator() { return new Exprespy; }
    static MStatus initialize();

    Exprespy();
    virtual ~Exprespy();

    MStatus compute(const MPlug&, MDataBlock&);
};

MTypeId Exprespy::id(EXPRESPY_NODE_ID);
MString Exprespy::name("exprespy");
MObject Exprespy::aCode;
MObject Exprespy::aCompiled;
MObject Exprespy::aInput;
MObject Exprespy::aOutput;


//------------------------------------------------------------------------------
/// Static: Initialization.
//------------------------------------------------------------------------------
MStatus Exprespy::initialize()
{
    MFnTypedAttribute fnTyped;
    MFnNumericAttribute fnNumeric;
    MFnGenericAttribute fnGeneric;

    // code
    aCode = fnTyped.create("code", "cd", MFnData::kString);
    fnTyped.setConnectable(false);
    CHECK_MSTATUS_AND_RETURN_IT( addAttribute(aCode) );

    // compiled
    aCompiled = fnNumeric.create("compile", "cm", MFnNumericData::kBoolean, false);
    fnNumeric.setWritable(false);
    fnNumeric.setStorable(false);
    CHECK_MSTATUS_AND_RETURN_IT( addAttribute(aCompiled) );

    // input
    aInput = fnGeneric.create("input", "i");
    fnGeneric.addAccept(MFnData::kNumeric);
    fnGeneric.addAccept(MFnNumericData::k3Double);
    fnGeneric.addAccept(MFnData::kMatrix);
    fnGeneric.addAccept(MFnData::kString);
    //fnGeneric.addAccept(MFnData::kNurbsCurve);
    //fnGeneric.addAccept(MFnData::kMesh);
    fnGeneric.setArray(true);
    CHECK_MSTATUS_AND_RETURN_IT(addAttribute(aInput));

    // output
    aOutput = fnGeneric.create("output", "o");
    fnGeneric.addAccept(MFnData::kNumeric);
    fnGeneric.addAccept(MFnNumericData::k3Double);
    fnGeneric.addAccept(MFnData::kMatrix);
    fnGeneric.addAccept(MFnData::kString);
    //fnGeneric.addAccept(MFnData::kNurbsCurve);
    //fnGeneric.addAccept(MFnData::kMesh);
    fnGeneric.setWritable(false);
    fnGeneric.setStorable(false);
    fnGeneric.setArray(true);
    CHECK_MSTATUS_AND_RETURN_IT(addAttribute(aOutput));

    // Set the attribute dependencies.
    attributeAffects(aCode, aCompiled);
    attributeAffects(aCode, aOutput);
    attributeAffects(aInput, aOutput);

    return MS::kSuccess;
}


//------------------------------------------------------------------------------
/// Constructor.
//------------------------------------------------------------------------------
Exprespy::Exprespy() :
    _globals(NULL),
    _base_locals(NULL),
    _input(NULL),
    _output(NULL),
    _type_Vector3(NULL),
    _type_Vector4(NULL),
    _type_EulerRot(NULL),
    _type_Matrix44(NULL),
    _locals(NULL),
    _codeobj(NULL)
{
}


//------------------------------------------------------------------------------
/// Destructor.
//------------------------------------------------------------------------------
Exprespy::~Exprespy()
{
    if (_globals) {
        PyGILState_STATE gil = PyGILState_Ensure();

        Py_XDECREF(_codeobj);
        Py_XDECREF(_locals);

        Py_DECREF(_output);
        Py_DECREF(_input);
        Py_DECREF(_base_locals);

        // PyDict_GetItemString �œ������^�I�u�W�F�N�g�͎Q�Ɛ����C���N�������g����Ă��Ȃ��̂� DECREF �s�v�B
        // PyModule_GetDict �œ��� _globals �͎Q�Ɛ����C���N�������g����Ă��Ȃ����A��� DECREF ���Ă͂����Ȃ��B

        PyGILState_Release(gil);
    }
}


//------------------------------------------------------------------------------
/// Computation.
//------------------------------------------------------------------------------
MStatus Exprespy::compute(const MPlug& plug, MDataBlock& block)
{
    if (plug == aCompiled) {
        return _compileCode(block);
    } else if (plug == aOutput) {
        return _executeCode(block);
    }
    return MS::kUnknownParameter;
}


//------------------------------------------------------------------------------
/// Compile a code.
//------------------------------------------------------------------------------
MStatus Exprespy::_compileCode(MDataBlock& block)
{
    MString code = block.inputValue(aCode).asString();

    //MGlobal::displayInfo("COMPILE");

    // Python �̏����J�n�iGIL�擾�j�B
    PyGILState_STATE gil = PyGILState_Ensure();

    // ��x�\�z�ς݂Ȃ�Â��R���p�C���ς݃R�[�h��j���B
    if (_globals) {
        Py_XDECREF(_codeobj);

    // ���߂ĂȂ���\�z����B
    } else {
        // __main__ �� __dict__ ���O���[�o���� (globals) �Ƃ��ē���B��������Ȃ��Ƒg�ݍ��݊֐�����g���Ȃ��B
        _globals = PyModule_GetDict(PyImport_AddModule("__main__"));
        if (_globals) {
            // ���[�J���� (locals) �̃x�[�X���\�z����B
            // ����͏����������Ȃ��悤�ɕێ����A�e�R�[�h�����ɂ͂���𕡐����ė��p����B
            _base_locals = PyDict_New();
            _input = PyDict_New();
            _output = PyDict_New();
            if (_base_locals && _input && _output) {
                // �m�[�h�̓��o�͂̂��߂� dict �𐶐��B
                PyDict_SetItemString(_base_locals, "IN", _input);
                PyDict_SetItemString(_base_locals, "OUT", _output);

                // ���W���[�����C���|�[�g�B
                PyObject* mod_api = _importModule("maya.api.OpenMaya");
                PyObject* mod_math = _importModule("math");
                PyObject* mod_cmds = _importModule("maya.cmds");
                PyObject* mod_mel = _importModule("maya.mel");

                // �C���|�[�g�������W���[�����R�[�h�̂��߂� locals �ɃZ�b�g�B
                PyDict_SetItemString(_base_locals, "api", mod_api ? mod_api : Py_None);
                PyDict_SetItemString(_base_locals, "math", mod_math ? mod_math : Py_None);
                PyDict_SetItemString(_base_locals, "cmds", mod_cmds ? mod_cmds : Py_None);
                PyDict_SetItemString(_base_locals, "mel", mod_mel ? mod_mel : Py_None);

                // Python API 2.0 ����N���X���擾���Ă����Btype �łȂ��Ƃ� callable �ł���΂悢���̂Ƃ���B
                if (mod_api) {
                    PyObject* api = PyModule_GetDict(mod_api);  // ����� DECREF �s�v�B
                    PyObject* o;
                    o = PyDict_GetItemString(api, "MVector");  // ������ DECREF �s�v�B
                    if (PyCallable_Check(o)) _type_Vector3 = o;
                    o = PyDict_GetItemString(api, "MPoint");
                    if (PyCallable_Check(o)) _type_Vector4 = o;
                    o = PyDict_GetItemString(api, "MEulerRotation");
                    if (PyCallable_Check(o)) _type_EulerRot = o;
                    o = PyDict_GetItemString(api, "MMatrix");
                    if (PyCallable_Check(o)) _type_Matrix44 = o;
                }

                // PyImport_Import �������͎̂Q�ƃJ�E���g�������Ă���̂Ō�n������B
                Py_XDECREF(mod_api);
                Py_XDECREF(mod_math);
                Py_XDECREF(mod_cmds);
                Py_XDECREF(mod_mel);

            } else {
                // ���\�z�Ɏ��s�B
                _globals = NULL;
                Py_CLEAR(_base_locals);
                Py_CLEAR(_input);
                Py_CLEAR(_output);
            }
        }
    }

    // ���[�J�����̕����ƁA�\�[�X�R�[�h�̃R���p�C���B
    if (_globals) {
        _locals = PyDict_Copy(_base_locals);
        if (_locals) {
            _codeobj = reinterpret_cast<PyCodeObject*>(Py_CompileString(code.asChar(), "exprespy_code", Py_file_input));
            if(PyErr_Occurred()){
                PyErr_Print();
            }
        }
    }

    // Python �̏����I���iGIL����j�B
    PyGILState_Release(gil);

    // �R���p�C���̐��ۂ��Z�b�g�B
    //MGlobal::displayInfo(_codeobj ? "successfull" : "failed");
    block.outputValue(aCompiled).set(_codeobj ? true : false);
    return MS::kSuccess;
}


//------------------------------------------------------------------------------
/// Execute a code.
//------------------------------------------------------------------------------
MStatus Exprespy::_executeCode(MDataBlock& block)
{
    block.inputValue(aCompiled);

    //MGlobal::displayInfo("EXECUTE");

    MArrayDataHandle hArrOutput = block.outputArrayValue(aOutput);

    if (_codeobj) {
        // GIL�擾�O�ɏ㗬�]�����I����B
        MArrayDataHandle hArrInput = block.inputArrayValue(aInput);
        // ���̒i�K�ŏ㗬���]������I����Ă���B

        // Python �̏����J�n�iGIL�擾�j�B
        PyGILState_STATE gil = PyGILState_Ensure();

        // .input[i] ����l�𓾂� IN[i] �ɃZ�b�g����B�����Ȃ����̂� None �Ƃ���B
        if (hArrInput.elementCount()) {
            do {
                const unsigned idx = hArrInput.elementIndex();

                MDataHandle hInput = hArrInput.inputValue();
                if (hInput.type() == MFnData::kNumeric) {
                    // �c�O�Ȃ��� generic �ł͐��l�^�̏ڍׂ͔��ʂł��Ȃ��B
                    _setInputScalar(idx, hInput.asGenericDouble());
                } else {
                    MObject data = hInput.data();
                    switch (data.apiType()) {
                    case MFn::kData3Double:
                        _setInputDouble3(idx, data);
                        break;
                    case MFn::kMatrixData:
                        _setInputMatrix(idx, data);
                        break;
                    case MFn::kStringData:
                        _setInputString(idx, hInput.asString());
                        break;
                    default:
                        //MGlobal::displayInfo(data.apiTypeStr());
                        _setDictValue(_input, idx, Py_None);
                    }
                }
            } while (hArrInput.next());
        }

        // �R���p�C���ς݃R�[�h�I�u�W�F�N�g�����s�B
        PyEval_EvalCode(_codeobj, _globals, _locals);
        if(PyErr_Occurred()){
            PyErr_Print();
        }

        // .output[i] �� OUT[i] �̒l���Z�b�g����B���݂��Ȃ����̂∵���Ȃ����̏ꍇ�͖�������B
        if (hArrOutput.elementCount()) {
            MMatrix mat;
            do {
                const unsigned idx = hArrOutput.elementIndex();

                PyObject* valo = _getDictValue(_output, idx);
                if (! valo) continue;

                if (PyFloat_Check(valo)) {
                    hArrOutput.outputValue().setGenericDouble(PyFloat_AS_DOUBLE(valo), true);

                } else if (PyInt_Check(valo)) {
                    hArrOutput.outputValue().setGenericInt(PyInt_AS_LONG(valo), true);

                } else if (PyLong_Check(valo)) {
                    hArrOutput.outputValue().setGenericInt(PyLong_AsLong(valo), true);

                } else if (PyBool_Check(valo)) {
                    hArrOutput.outputValue().setGenericBool(valo == Py_True, true);

                } else if (PyString_Check(valo)) {
                    hArrOutput.outputValue().set(MString(PyString_AsString(valo)));

                } else if (PyUnicode_Check(valo)) {
                    //PyObject* es = PyUnicode_AsUTF8String(valo);
                    PyObject* es = PyUnicode_AsEncodedString(valo, Py_FileSystemDefaultEncoding, NULL);
                    if (es) {
                        hArrOutput.outputValue().set(MString(PyString_AsString(es)));
                        Py_DECREF(es);
                    } else {
                        hArrOutput.outputValue().set(MString());
                    }

                } else if ((_type_Vector3 && PyObject_IsInstance(valo, _type_Vector3))
                 || (_type_Vector4 && PyObject_IsInstance(valo, _type_Vector4))
                 || (_type_EulerRot && PyObject_IsInstance(valo, _type_EulerRot))) {
                    MFnNumericData fnOutData;
                    MObject data = fnOutData.create(MFnNumericData::k3Double);
                    _getDouble3ValueFromPyObject(fnOutData, valo);
                    hArrOutput.outputValue().set(data);

                } else if (_type_Matrix44 && PyObject_IsInstance(valo, _type_Matrix44)) {
                    _getMatrixValueFromPyObject(mat, valo);
                    hArrOutput.outputValue().set(MFnMatrixData().create(mat));
                }

                //Py_DECREF(valo);  // ����͂���Ă͂Ȃ�Ȃ��BPyDict_GetItem �͎Q�ƃJ�E���g�𑝂₳�Ȃ��B
            } while (hArrOutput.next());
        }

        // ����̂��߂� IN �� OUT ���N���A���Ă����B
        PyDict_Clear(_input);
        PyDict_Clear(_output);

        // Python �̏����I���iGIL����j�B
        PyGILState_Release(gil);
    }

    return hArrOutput.setAllClean();
}


//------------------------------------------------------------------------------
/// _input dict �ɃX�J���[���l���Z�b�g����B
//------------------------------------------------------------------------------
void Exprespy::_setInputScalar(int key, const double& val)
{
    PyObject* keyo = PyInt_FromLong(key);
    if (keyo) {
        // ���͂���^���ʏo���Ȃ����߁A���߂Đ����Ǝ����𔻕ʂ���B
        int i = static_cast<int>(val);
        if (static_cast<double>(i) == val) {
            _setDictStealValue(_input, keyo, PyInt_FromLong(i));
        } else {
            _setDictStealValue(_input, keyo, PyFloat_FromDouble(val));
        }
    }
}


//------------------------------------------------------------------------------
/// _input dict �ɕ�������Z�b�g����B
//------------------------------------------------------------------------------
void Exprespy::_setInputString(int key, const MString& str)
{
    PyObject* keyo = PyInt_FromLong(key);
    if (keyo) {
        //_setDictStealValue(_input, keyo, PyString_FromString(str.asChar()));
        _setDictStealValue(_input, keyo, PyUnicode_FromString(str.asUTF8()));
    }
}


//------------------------------------------------------------------------------
/// _input dict �� double3 ��K�؂Ȍ^�ɂ��ăZ�b�g����B
//------------------------------------------------------------------------------
void Exprespy::_setInputDouble3(int key, MObject& data)
{
    PyObject* keyo = PyInt_FromLong(key);
    if (! keyo) return;

    PyObject* valo = NULL;
    if (_type_Vector3) {
        double x, y, z;
        MFnNumericData(data).getData(x, y, z);
        PyObject* args = Py_BuildValue("(ddd)", x, y, z);
        if (args) {
            valo = PyObject_CallObject(_type_Vector3, args);
            Py_DECREF(args);
        }
    }
    _setDictStealValue(_input, keyo, valo);
}


//------------------------------------------------------------------------------
/// _input dict �� matrix ��K�؂Ȍ^�ɂ��ăZ�b�g����B
//------------------------------------------------------------------------------
void Exprespy::_setInputMatrix(int key, MObject& data)
{
    PyObject* keyo = PyInt_FromLong(key);
    if (! keyo) return;

    PyObject* valo = NULL;
    if (_type_Matrix44) {
        MMatrix m = MFnMatrixData(data).matrix();
        PyObject* args = Py_BuildValue("(N)", Py_BuildValue(
            "(dddddddddddddddd)",
            m.matrix[0][0], m.matrix[0][1], m.matrix[0][2], m.matrix[0][3],
            m.matrix[1][0], m.matrix[1][1], m.matrix[1][2], m.matrix[1][3],
            m.matrix[2][0], m.matrix[2][1], m.matrix[2][2], m.matrix[2][3],
            m.matrix[3][0], m.matrix[3][1], m.matrix[3][2], m.matrix[3][3]
        ));
        if (args) {
            valo = PyObject_CallObject(_type_Matrix44, args);
            Py_DECREF(args);
        }
    }
    _setDictStealValue(_input, keyo, valo);
}


//=============================================================================
//  ENTRY POINT FUNCTIONS
//=============================================================================
MStatus initializePlugin(MObject obj)
{ 
    static const char* VERSION = "1.0.20161002";
    static const char* VENDER  = "Ryusuke Sasaki";

    MFnPlugin plugin(obj, VENDER, VERSION, "Any");

    MStatus stat = plugin.registerNode(Exprespy::name, Exprespy::id, Exprespy::creator, Exprespy::initialize);
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus stat = MFnPlugin(obj).deregisterNode(Exprespy::id);
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    return MS::kSuccess;
}

