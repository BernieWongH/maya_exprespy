# maya_exprespy
Maya �� Python �ɂ��G�N�X�v���b�V�����@�\��񋟂���m�[�h�̃v���O�C���ł��B
exprespy�i�G�N�X�v���X�p�C�j�ƌĂт܂��B

![SS](/exprespy.png)


##����
�{�v���O�C���ł͑��x���d�����A�m�[�h�� C++ �Ŏ�������Ă��܂��B
�܂��A����ɂ���āAPython �ɂ��G�N�X�v���b�V�����R�[�h�͈�x�����R���p�C�����ꃁ������ɕێ�����A
�ȍ~�̓R���p�C���ς݂̃R�[�h�I�u�W�F�N�g�����s����d�g�݂��������Ă��܂��B
�܂�A���s���̌������ǂ����̂ƂȂ��Ă��܂��B

�܂��A�W���̃G�N�X�v���b�V�����@�\�Ɠ��l�ɁA�G�f�B�^�i�A�g���r���[�g�G�f�B�^�j��ł́A
���ۂ̃m�[�h�E�A�g���r���[�g���ł̃R�[�f�B���O���\�ł��B
�����́A�W���@�\�Ɠ��l�ɁA���ۂ̓m�[�h�̃R�l�N�V�����ɒu����������悤�ɂȂ��Ă��܂��B

�������A�P�ʕϊ��̋@�\�͂Ȃ��A�����P�ʂŒ��ڈ����܂��B
�������S�Ҍ����łȂ������ł����A����������d�����鏊�Ȃł��B

����ɁAPython API 2.0 �̌^�ɑΉ����Adouble3 �� matrix �A�g���r���[�g�𒼐ړ��o�͂��邱�Ƃ��\�ł��B


##�ގ��Z�p
Python �ŃG�N�X�v���b�V������������悤�ɂ���v���O�C����
[SOuP](http://www.soup-dev.com/)
�Ɋ܂܂��
[pyExpression](http://www.soup-dev.com/wiki/PyExpression.html)
�m�[�h���L���ł��B
�������A����͑S�� Python �Ŏ�������Ă���A
Python �̃G�N�X�v���b�V�����R�[�h�� exec() �֐��ɂ���Ď��s�����d�g�݂ŁA
���s���邽�тɃR�[�h���R���p�C�����邽�ߌ����I�ł͂���܂���B

�܂��A�W���ŕt������ MASH �ɂ� Python �m�[�h������APython �Ń��[�V�����O���t�B�b�N�X�𐧌䂷�邱�Ƃ��o���܂��B
�����A����� particle �f�[�^�𐧌䂷��d�g�݂ŁA����ɓ������ꂽ���\�͑f���炵���ł����A�ėp�I�ł͂���܂���B


##�f�B���N�g���\��
* examples: Maya�V�[���̗�B
* plug-ins: �r���h�ς݃v���O�C���o�C�i���B
* python: �T�|�[�g python ���W���[���B
* scripts: �T�|�[�g mel �X�N���v�g�B
* srcs: C++�\�[�X�R�[�h�B


##�C���X�g�[�����@
* plug-ins �t�H���_�ɂ���v���b�g�t�H�[���ƃo�[�W�������Ƃ̃t�H���_�Ɏ��߂��Ă���t�@�C����
  MAYA_PLUG_IN_PATH �̒ʂ����t�H���_�ɃR�s�[����B

* python �t�H���_�ɂ���t�@�C���� PYTHONPATH �̒ʂ����t�H���_�ɃR�s�[����B

* scripts �t�H���_�ɂ���t�@�C���� MAYA_SCRIPT_PATH �̒ʂ����t�H���_�ɃR�s�[����B


##�g�p���@
�G�N�X�v���b�V�������L�q����ɂ́A�܂� exprespy �m�[�h�𐶐����܂��B
�ȉ��̂ǂ��炩�̕��@�����p�ł��܂��B

* �v���O�C�������[�h���Ă���A�ȉ��� mel �R�[�h�����s�B

  ```
  createNode exprespy;
  ```

* �ȉ��� python �R�[�h�����s�i�v���O�C���̓��[�h����Ă��Ȃ��Ă�OK�j�B

  ```
  import exprespy
  exprespy.create()
  ```

���̌�A�������ꂽ exprespy �m�[�h�̃A�g���r���[�g�G�f�B�^�ɃR�[�h����͂��܂��B


##�T���v���V�[��
* constraints.ma

  �l�X�ȃR���X�g���C���@�\���G�N�X�v���b�V�����Ŏ���������B
  position�Aorient�A�A�b�v�I�u�W�F�N�g������ aim�A�A�b�v�I�u�W�F�N�g�L��� aim ���������Ă��܂��B


##�ڍ׎d�l

* ���̓A�g���r���[�g�̌^�͈ȉ��ɑΉ����Ă��܂��B

  - �l�X�ȃX�J���[���l�^

    Python ��ł� float �� int �ɂȂ�܂��iPython �ł� float �� double �̋�ʂ͂���܂���j�B
    ���͐��l�^�̔��ʂ������ɂ͏o���Ȃ����� bool �� int �ɂȂ�܂��B

  - string �^

    Python ��ł� unicode �ɂȂ�܂��B

  - double3 �^

    Python ��ł� API 2.0 �� MVector �ɂȂ�܂��B
    ���̌�AMPoint �� MEulerRotation ���ɕϊ�����̂����R���݂ł��B

  - matrix �^

    Python ��ł� API 2.0 �� MMatrix �ɂȂ�܂��B

* �o�̓A�g���r���[�g�̌^�͈ȉ��ɑΉ����Ă��܂��B

  - �l�X�ȃX�J���[���l�^

    Python �̌^�ɉ����āA�ȉ��̂悤�ɃA�g���r���[�g�^�����܂�܂��B

    - bool -> bool
    - int -> int
    - long int -> int
    - float -> double

  - string �^

    Python ��� str �� unicode �� string �^�ɂȂ�܂��B

  - double3 �^

    Python ��� API 2.0 �� MVector, MPoint, MEulerRotation �� double3 �ɂȂ�܂��B
    ���̂��� MPoint �� w �� MEulerRotation �� order �͂��̂܂܂��Ǝ̂Ă��܂��̂ŁA
    �K�v�Ȃ�ʃA�g���r���[�g�ɏo���Ă��������B

  - matrix �^

    Python ��� API 2.0 �� MMatrix �� matrix �^�ɂȂ�܂��B

* ���W���[��

  Python �G�N�X�v���b�V�����R�[�h�ł́A�ȉ��̃��W���[�����C���|�[�g�ς݂Ŏg�����ԂƂȂ��Ă��܂��B

  ```
  import math
  import maya.api.OpenMaya as api
  import maya.cmds as cmds
  import maya.mel as mel
  ```

  ���ɕK�v�ȃ��W���[��������Ύ��R�� import ���Ă�������
  �i��ʓI�� import ���̕��ׂ͍��߂Ȃ̂ŁA�����s�P�ʂōs���̂͏������Ӂj�B

* �X�R�[�v

  Python �G�N�X�v���b�V�����R�[�h�ł́AMaya Python �̃O���[�o���X�R�[�v��̖��O�ɎQ�Əo���܂��B
  �������A�V���Ɏg�p�����ϐ��ȂǂŁAMaya �̃O���[�o���X�R�[�v�͉���܂���B

  ���[�J���X�R�[�v�́A�R�[�h���ҏW����R���p�C�����������܂ŕێ�����܂��B
  ����ɂ���āA�Ⴆ�΁A��Ԃ��L�����čŏ��̈�񂾂����s���鏈�����L�q�����肷�邱�Ƃ��\�ł��B

* �R�[�h�͂ǂ̂悤�ɕۑ������̂�

  exprespy �m�[�h�� code �Ƃ��� string �A�g���r���[�g�ɕۑ�����܂��B
  �R�[�h���̃A�g���r���[�g�Q�Ƃ� ```IN[index]``` �� ```OUT[index]``` �̂悤�ȃv���[�X�z���_�ɒu�������܂��B
  ���ۂ̃A�g���r���[�g�́A�����ɑΉ����� input[] �� output[] �̃v���O�ɐڑ�����܂��B

  �A�g���r���[�g�G�f�B�^��Ɍ����Ă���R�[�h�́A�v���[�X�z���_��u�����������̃R�[�h�ł��B
  ���̏����� exprespy �� Python ���W���[�����s���Ă��܂��B


##��������
* �R�[�h���ɏ������A�g���r���[�g�Q�Ƃ̓p�[�X����Ď��ۂ̃R�l�N�V�����ƂȂ�킯�ł����A
  �R�[�h�̃R�����g�����ʂ��Ă��Ȃ����߁A
  �R�����g���ɂ����ۂ̃v���O�Ƀ}�b�`���閼�O������ƃR�l�N�V�����ɒu���������Ă��܂��܂��B
  ���s���ʂ����������Ȃ�킯�ł͂���܂��񂪁A�]�v�ȎQ�Ƃ������邽�߁A���ʂɂ͂Ȃ�܂��B

* GUI�i�A�g���r���[�g�G�f�B�^�j�́A���܂�C������ď����Ă��Ȃ����߁A���ɂ������ȋ��������邩���m��܂���B

* �d�l�ɏ������A�g���r���[�g�̓��o�͂ɂ����Ή����Ă��܂��񂪁A
  �����I�ɂ� mesh�AnurbsCurve�AnurbsSurface ���̓��o�͂��\�ɂ������ƍl���Ă͂��܂��B
  �����A������x�������̂ŁA�܂��͔�Ή��ł��B
  �i�f�[�^�I�u�W�F�N�g�� C++ API ���� Python API �ւ̌����I�ɕϊ������@���v�����Ȃ��c�j


##��������
* 2016.10.2: ����

