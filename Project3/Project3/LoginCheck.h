#pragma once
#ifndef __LoginCheckException
#define __LoginCheckException

#include <exception>
#include <new>
#include <string>

/*
�N���X��:LoginCheckException
�T�v:���O�C���`�F�b�N�G���[���̗�O�N���X
�쐬��:���c�C�V
�쐬��:8��16��(��)
*/
class LoginCheckException :private std::exception {
private:
	//�G���[�������̃N�b�L�[�̒l��ێ����郁���o������
	std::string cookieValue;
	//�G���[�������̃Z�b�V�����̒l��ێ����郁���o������
	std::string sessionValue;

public:
	//�R���X�g���N�^
	LoginCheckException(std::string cookiValue, std::string sessionValue) {
		//�����̃N�b�L�[�̕�����������o�ɕێ�
		this->cookieValue = cookieValue;
		//�����̃Z�b�V�����̕�����������o�ɕێ�
		this->sessionValue = sessionValue;
	}

	/*
	�֐���:checkLoginState
	�T�v:���O�C����Ԃ𒲂ׂĐ��l�ŕԂ�
	�쐬��:���c�C�V
	�쐬��:
	*/
	int checkLoginState() {
		//�ԋp���郍�O�C���̏�Ԃ�\���ϐ�
		int retState = 0;

		//�N�b�L�[�ƃZ�b�V������������Ǝ擾�ł��Ă���Ƃ�
		if (cookieValue != "", sessionValue != "") {
			//���O�C����Ԃł��邱�Ƃ�����1����
			retState = 1;
		}

		//���O�C���̏�Ԃ�ԋp����
		return retState;
	}
};


#endif // !__LoginCheckException
