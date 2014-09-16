/*
    tutorial037 - hierarchy
*/

#define GLM_FORCE_RADIANS

#include <stdio.h>
#include <string.h>
#include <stack>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective 
#include "glm/gtc/type_ptr.hpp"         // glm::value_ptr 
#include "textfile.h"

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))

GLuint v, f, p;  //shader identifiers: vertex, fragment, program
GLuint positionAttrib;
GLuint colorAttrib;

GLuint modelToCameraMatrixUnif;
GLuint cameraToClipMatrixUnif;

    glm::mat4 cameraToClipMatrix(1.0f);
    
    glm::mat4 modelToCameraMatrix(1.0f);

float CalcFrustumScale(float fFovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fFovRad = fFovDeg * degToRad;
	return 1.0f / tan(fFovRad / 2.0f);
}

float fFrustumScale = CalcFrustumScale(45.0f);


GLuint VBO;
GLuint IBO;
GLuint vao;

//??? needed???
float edgesize = 3.f;    // size of the object 
float offset = -10.0f; 


const int numberOfVertices = 24;

#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f, 1.0f

#define YELLOW_COLOR 1.0f, 1.0f, 0.0f, 1.0f
#define CYAN_COLOR 0.0f, 1.0f, 1.0f, 1.0f
#define MAGENTA_COLOR 	1.0f, 0.0f, 1.0f, 1.0f


const float vertexData[] =
{
	//Front
	+1.0f, +1.0f, +1.0f,
	+1.0f, -1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,

	//Top
	+1.0f, +1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,
	-1.0f, +1.0f, -1.0f,
	+1.0f, +1.0f, -1.0f,

	//Left
	+1.0f, +1.0f, +1.0f,
	+1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,
	+1.0f, -1.0f, +1.0f,

	//Back
	+1.0f, +1.0f, -1.0f,
	-1.0f, +1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,

	//Bottom
	+1.0f, -1.0f, +1.0f,
	+1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, +1.0f,

	//Right
	-1.0f, +1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, +1.0f, -1.0f,


	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,

	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,

	RED_COLOR,
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,

	YELLOW_COLOR,
	YELLOW_COLOR,
	YELLOW_COLOR,
	YELLOW_COLOR,

	CYAN_COLOR,
	CYAN_COLOR,
	CYAN_COLOR,
	CYAN_COLOR,

	MAGENTA_COLOR,
	MAGENTA_COLOR,
	MAGENTA_COLOR,
	MAGENTA_COLOR,
};

const GLshort indexData[] =
{
	0, 1, 2,
	2, 3, 0,

	4, 5, 6,
	6, 7, 4,

	8, 9, 10,
	10, 11, 8,

	12, 13, 14,
	14, 15, 12,

	16, 17, 18,
	18, 19, 16,

	20, 21, 22,
	22, 23, 20,
};


//======================== SetShaders ========================================
static void SetShaders()
{
    char *vs = NULL,*fs = NULL;
    
    v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

    vs = textFileRead("basic.vert");
	fs = textFileRead("basic.frag");
	
	const char * ff = fs;
	const char * vv = vs;
	
	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);
	
	free(vs);free(fs);
	
	glCompileShader(v);
	glCompileShader(f);

	p = glCreateProgram();
	
	if (p == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }
	
	glAttachShader(p,f);
	glAttachShader(p,v);

	glLinkProgram(p);


	
}	
//======================== end SetShaders ===================================

//----------------------- InitializeProgram ---------------------------------
void InitializeProgram()
{
    
     SetShaders();
     glUseProgram(p);   
     
     positionAttrib = glGetAttribLocation(p, "position");
	 colorAttrib = glGetAttribLocation(p, "color");
     modelToCameraMatrixUnif = glGetUniformLocation(p, "modelToCameraMatrix");
     cameraToClipMatrixUnif = glGetUniformLocation(p, "cameraToClipMatrix");

   

    // Set perspective projection matrix - cameraToClipMatrix
	float fzNear = 1.0f; float fzFar = 100.0f;
	
	printf("fFrustumScale =  %f \n", fFrustumScale); 

	cameraToClipMatrix[0].x = fFrustumScale;
	cameraToClipMatrix[1].y = fFrustumScale;
	cameraToClipMatrix[2].z = (fzFar + fzNear) / (fzNear - fzFar);
	cameraToClipMatrix[2].w = -1.0f;
	cameraToClipMatrix[3].z = (2 * fzFar * fzNear) / (fzNear - fzFar);

	 
    // send transformation matrices to the shader
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, &cameraToClipMatrix[0][0]);
	glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, &modelToCameraMatrix[0][0]);
	glUseProgram(0); 
}


//------------------------------ CreateVAO ---------------------------
static void CreateVAO()
{ 
        
 	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	size_t colorDataOffset = sizeof(float) * 3 * numberOfVertices;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(positionAttrib);
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBindVertexArray(0);

}
//------------------------------ end CreateVAO ----------------------

//=================== Hierarchy structure ====================================

inline float DegToRad(float fAngDeg)
{
	const float fDegToRad = 3.14159f * 2.0f / 360.0f;
	return fAngDeg * fDegToRad;
}

inline float Clamp(float fValue, float fMinValue, float fMaxValue)
{
	if(fValue < fMinValue)
		return fMinValue;

	if(fValue > fMaxValue)
		return fMaxValue;

	return fValue;
}

glm::mat3 RotateX(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[1].y = fCos; theMat[2].y = -fSin;
	theMat[1].z = fSin; theMat[2].z = fCos;
	return theMat;
}

glm::mat3 RotateY(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[0].x = fCos; theMat[2].x = fSin;
	theMat[0].z = -fSin; theMat[2].z = fCos;
	return theMat;
}

glm::mat3 RotateZ(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[0].x = fCos; theMat[1].x = -fSin;
	theMat[0].y = fSin; theMat[1].y = fCos;
	return theMat;
}

class MatrixStack
{
public:
	MatrixStack()
		: m_currMat(1.0f)
	{
	}

	const glm::mat4 &Top()
	{
		return m_currMat;
	}

	void RotateX(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateX(fAngDeg));
	}

	void RotateY(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateY(fAngDeg));
	}

	void RotateZ(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateZ(fAngDeg));
	}

	void Scale(const glm::vec3 &scaleVec)
	{
		glm::mat4 scaleMat(1.0f);
		scaleMat[0].x = scaleVec.x;
		scaleMat[1].y = scaleVec.y;
		scaleMat[2].z = scaleVec.z;

		m_currMat = m_currMat * scaleMat;
	}

	void Translate(const glm::vec3 &offsetVec)
	{
		glm::mat4 translateMat(1.0f);
		translateMat[3] = glm::vec4(offsetVec, 1.0f);

		m_currMat = m_currMat * translateMat;
	}

	void Push()
	{
		m_matrices.push(m_currMat);
	}

	void Pop()
	{
		m_currMat = m_matrices.top();
		m_matrices.pop();
	}

private:
	glm::mat4 m_currMat;
	std::stack<glm::mat4> m_matrices;
};

class Hierarchy
{
public:
	Hierarchy()
		: posBase(glm::vec3(3.0f, -5.0f, -40.0f))
		, angBase(-45.0f)
		, posBaseLeft(glm::vec3(2.0f, 0.0f, 0.0f))
		, posBaseRight(glm::vec3(-2.0f, 0.0f, 0.0f))
		, posBaseLeftSmall(glm::vec3(1.5f, 0.5f, 0.0f))
		, posBaseRightSmall(glm::vec3(-1.5f, 0.5f, 0.0f))
		, scaleBaseZ(3.0f)
		, scaleBaseZ2(2.5f)
		, angUpperArm(-33.75f)
		, sizeUpperArm(9.0f)
		, posLowerArm(glm::vec3(0.0f, 0.0f, 8.0f))
		, angLowerArm(146.25f)
		, lenLowerArm(5.0f)
		, widthLowerArm(1.5f)
		, posWrist(glm::vec3(0.0f, 0.0f, 5.0f))
		, angWristRoll(0.0f)
		, angWristPitch(67.5f)
		, lenWrist(2.0f)
		, widthWrist(2.0f)
		, posLeftFinger(glm::vec3(1.0f, 0.0f, 1.0f))
		, posRightFinger(glm::vec3(-1.0f, 0.0f, 1.0f))
		, angFingerOpen(180.0f)
		, lenFinger(2.0f)
		, widthFinger(0.5f)
		, angLowerFinger(45.0f)
	{}

	void Draw()
	{
		MatrixStack modelToCameraStack;

		glUseProgram(p);
		glBindVertexArray(vao);

		modelToCameraStack.Translate(posBase);
		modelToCameraStack.RotateY(angBase);

		//Draw left base.
		{
			modelToCameraStack.Push();
			modelToCameraStack.Translate(posBaseLeft);
			modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
		}
		
		{
			modelToCameraStack.Push();
			modelToCameraStack.Translate(posBaseLeftSmall);
			modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
		}

		//Draw right base.
		{
			modelToCameraStack.Push();
			modelToCameraStack.Translate(posBaseRight);
			modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
		}
		
		{
			modelToCameraStack.Push();
			modelToCameraStack.Translate(posBaseRightSmall);
			modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, scaleBaseZ));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
		}

		//Draw main arm.
		DrawUpperArm(modelToCameraStack);

		glBindVertexArray(0);
		glUseProgram(0);
	}

#define STANDARD_ANGLE_INCREMENT 11.25f
#define SMALL_ANGLE_INCREMENT 9.0f

	void AdjBase(bool bIncrement)
	{
		angBase += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angBase = fmodf(angBase, 360.0f);
	}

	void AdjUpperArm(bool bIncrement)
	{
		angUpperArm += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angUpperArm = Clamp(angUpperArm, -90.0f, 0.0f);
	}

	void AdjLowerArm(bool bIncrement)
	{
		angLowerArm += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angLowerArm = Clamp(angLowerArm, 0.0f, 146.25f);
	}

	void AdjWristPitch(bool bIncrement)
	{
		angWristPitch += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angWristPitch = Clamp(angWristPitch, 0.0f, 90.0f);
	}

	void AdjWristRoll(bool bIncrement)
	{
		angWristRoll += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angWristRoll = fmodf(angWristRoll, 360.0f);
	}

	void AdjFingerOpen(bool bIncrement)
	{
		angFingerOpen += bIncrement ? SMALL_ANGLE_INCREMENT : -SMALL_ANGLE_INCREMENT;
		angFingerOpen = Clamp(angFingerOpen, 9.0f, 180.0f);
	}

	void WritePose()
	{
		printf("angBase:\t%f\n", angBase);
		printf("angUpperArm:\t%f\n", angUpperArm);
		printf("angLowerArm:\t%f\n", angLowerArm);
		printf("angWristPitch:\t%f\n", angWristPitch);
		printf("angWristRoll:\t%f\n", angWristRoll);
		printf("angFingerOpen:\t%f\n", angFingerOpen);
		printf("\n");
	}

private:
	void DrawFingers(MatrixStack &modelToCameraStack)
	{
		//Draw left finger
		modelToCameraStack.Push();
		modelToCameraStack.Translate(posLeftFinger);
		modelToCameraStack.RotateY(angFingerOpen);

		modelToCameraStack.Push();
		modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
		modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
		glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
		modelToCameraStack.Pop();

		{
			//Draw left lower finger
			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger));
			modelToCameraStack.RotateY(-angLowerFinger);

			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
			modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
			
			modelToCameraStack.Pop();
			
			
			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.5f, 0.0f, lenFinger / 1.5f));
			modelToCameraStack.RotateY(-angLowerFinger*1.5f);

			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 0.8f));
			modelToCameraStack.Scale(glm::vec3(widthFinger / 2.5f, widthFinger/ 2.5f, lenFinger / 2.5f));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();

			modelToCameraStack.Pop();
		}
		
		modelToCameraStack.Pop();
		
		//Draw right finger
		modelToCameraStack.Push();
		modelToCameraStack.Translate(posRightFinger);
		modelToCameraStack.RotateY(-angFingerOpen);

		modelToCameraStack.Push();
		modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
		modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
		glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
		modelToCameraStack.Pop();

		{
			//Draw right lower finger
			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger));
			modelToCameraStack.RotateY(angLowerFinger);

			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 2.0f));
			modelToCameraStack.Scale(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
			
			modelToCameraStack.Pop();
			
			
			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(-0.5f, 0.0f, lenFinger / 1.5f));
			modelToCameraStack.RotateY(angLowerFinger*1.5f);

			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenFinger / 0.8f));
			modelToCameraStack.Scale(glm::vec3(widthFinger / 2.5f, widthFinger/ 2.5f, lenFinger / 2.5f));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
			
			modelToCameraStack.Pop();

		}
		
		modelToCameraStack.Pop();
	}

	void DrawWrist(MatrixStack &modelToCameraStack)
	{
		modelToCameraStack.Push();
		modelToCameraStack.Translate(posWrist);
		modelToCameraStack.RotateZ(angWristRoll);
		modelToCameraStack.RotateX(angWristPitch);

		modelToCameraStack.Push();
		modelToCameraStack.Scale(glm::vec3(widthWrist / 2.0f, widthWrist/ 2.0f, lenWrist / 2.0f));
		glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
		modelToCameraStack.Pop();

		DrawFingers(modelToCameraStack);

		modelToCameraStack.Pop();
	}

	void DrawLowerArm(MatrixStack &modelToCameraStack)
	{
		modelToCameraStack.Push();
		modelToCameraStack.Translate(posLowerArm);
		modelToCameraStack.RotateX(angLowerArm);

		modelToCameraStack.Push();
		modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, lenLowerArm / 2.0f));
		modelToCameraStack.Scale(glm::vec3(widthLowerArm / 2.0f, widthLowerArm / 2.0f, lenLowerArm / 2.0f));
		glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
		modelToCameraStack.Pop();

		DrawWrist(modelToCameraStack);

		modelToCameraStack.Pop();
	}

	void DrawUpperArm(MatrixStack &modelToCameraStack)
	{
		modelToCameraStack.Push();
		modelToCameraStack.RotateX(angUpperArm);

		{
			modelToCameraStack.Push();
			modelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, (sizeUpperArm / 2.0f) - 1.0f));
			modelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, sizeUpperArm / 2.0f));
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelToCameraStack.Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			modelToCameraStack.Pop();
		}

		DrawLowerArm(modelToCameraStack);

		modelToCameraStack.Pop();
	}

	glm::vec3		posBase;
	float			angBase;

	glm::vec3		posBaseLeft, posBaseRight;
	float			scaleBaseZ;
	
	glm::vec3		posBaseLeftSmall, posBaseRightSmall;
	float			scaleBaseZ2;

	float			angUpperArm;
	float			sizeUpperArm;

	glm::vec3		posLowerArm;
	float			angLowerArm;
	float			lenLowerArm;
	float			widthLowerArm;

	glm::vec3		posWrist;
	float			angWristRoll;
	float			angWristPitch;
	float			lenWrist;
	float			widthWrist;

	glm::vec3		posLeftFinger, posRightFinger;
	float			angFingerOpen;
	float			lenFinger;
	float			widthFinger;
	float			angLowerFinger;
};


Hierarchy g_armature;

//==================== end Hierarchy structure =================================



//------------------ Init ----------------------------------------------------
static void Init()
{
    // get data from the graphics driver n
	printf("===================================================\n");
	printf("Vendor:         %s\n", glGetString(GL_VENDOR));
	printf("Graphics card:  %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version:   %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("GLEW version:   %s\n", glewGetString(GLEW_VERSION));
	printf("===================================================\n");

	// define background color - clear the window
    glClearColor(0.0f, 0.3f, 0.0f, 0.0f);
    
    glEnable(GL_CULL_FACE);    // 
    glCullFace(GL_BACK);       // Do not render back side
    glFrontFace(GL_CW);       // Front side - counter clock wise
    
    //glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
    
    InitializeProgram();
    CreateVAO();
    
    glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}


//==============================================================================

static void Display()
{
    	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_armature.Draw();

	glutSwapBuffers();
	glutPostRedisplay();
}


void Reshape (int w, int h)
{
    cameraToClipMatrix[0].x = fFrustumScale * (h / (float)w);
	cameraToClipMatrix[1].y = fFrustumScale;

	glUseProgram(p);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, &cameraToClipMatrix[0][0]);
	glUseProgram(0);
    
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}


void SpecialKeys(int key, int x, int y)
    {
	if(key == GLUT_KEY_UP)
		{fFrustumScale += 1;}

	if(key == GLUT_KEY_DOWN)
		{fFrustumScale -= 1;}
	
	if(key == GLUT_KEY_LEFT)
		{}

	if(key == GLUT_KEY_RIGHT)
		{}

	glutPostRedisplay();
	}

void Keyboard(unsigned char key, int x, int y)
    {
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	case 'a': g_armature.AdjBase(true); break;
	case 'd': g_armature.AdjBase(false); break;
	case 'w': g_armature.AdjUpperArm(false); break;
	case 's': g_armature.AdjUpperArm(true); break;
	case 'r': g_armature.AdjLowerArm(false); break;
	case 'f': g_armature.AdjLowerArm(true); break;
	case 't': g_armature.AdjWristPitch(false); break;
	case 'g': g_armature.AdjWristPitch(true); break;
	case 'z': g_armature.AdjWristRoll(true); break;
	case 'c': g_armature.AdjWristRoll(false); break;
	case 'q': g_armature.AdjFingerOpen(true); break;
	case 'e': g_armature.AdjFingerOpen(false); break;
	case 32: g_armature.WritePose(); break;
	}
	
	
	glutPostRedisplay();
	}



int main(int argc, char** argv)
{
    // set the right rendering context
	glutInitContextVersion(3, 1);
	glutInitContextFlags(GLUT_DEBUG);
	// set the OpenGL profile
	glutInitContextProfile(GLUT_CORE_PROFILE);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("tutorial037 - hierarchy");
    
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }

    Init();
   
    printf("GL version: %s\n", glGetString(GL_VERSION));


    glutDisplayFunc(Display);
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(Keyboard);

    glutMainLoop();

    return 0;
}
