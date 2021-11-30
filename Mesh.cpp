#include "Mesh.h"

	ModelData modelData;
	GLuint loc1, loc2, loc3;
	GLuint vao = 0;
	GLuint vertexBufferP = 0;
	GLuint vertexBufferN = 0;
	unsigned int texture;
	int x;
	int y;
	int z;


	void Mesh::bind() {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferP);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferN);
	}

	ModelData Mesh::load_mesh(const char* file_name) {
		ModelData modelD;
		const aiScene* scene = aiImportFile(
			file_name, aiProcess_Triangulate | aiProcess_PreTransformVertices
		);

		if (!scene) {
			fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
			return modelD;
		}

		for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
			const aiMesh* mesh = scene->mMeshes[m_i];
			printf("    %i vertices in mesh\n", mesh->mNumVertices);
			modelD.mPointCount += mesh->mNumVertices;
			for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
				if (mesh->HasPositions()) {
					const aiVector3D* vp = &(mesh->mVertices[v_i]);
					modelD.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
				}
				if (mesh->HasNormals()) {
					const aiVector3D* vn = &(mesh->mNormals[v_i]);
					modelD.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
				}
				if (mesh->HasTextureCoords(0)) {
					const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
					modelD.mTextureCoords.push_back(vec2(vt->x, vt->y));
				}

			}
		}

		aiReleaseImport(scene);
		return modelD;
	}

	GLuint shaderProgram;

	void Mesh::create(const char* mesh_name, GLuint shader, const char* textureName) {
		setShaders(shader);
		init(mesh_name, textureName);
		setData();


		if (mesh_name == "models/iceberg.dae")
		{
			 x = (rand() % 200) - 95;
			 y = 40;
			 z = (rand() % 80) - 95;
		}
		else{
			float offset = 2.1;

			x = (rand() % 300) - 150;
			y = (rand() % 30) - 15;
			z = (rand() % 130) * -1;
			
			
		}
	}

	void Mesh::displayIce(int matrix_location) {
		//Draw Iceberg Object


		mat4 modelIce = identity_mat4();
		modelIce = translate(modelIce, vec3(x,y,z));
		modelIce = scale(modelIce, vec3(100, 5, 100));
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelIce.m);
		render();
	}

	void Mesh::displayTrash(int matrix_location) {
		//Draw Trash Object
		float scaler = 0.3;
		mat4 modelTrash = identity_mat4();
		modelTrash = scale(modelTrash, vec3(scaler, scaler, scaler));
		modelTrash = translate(modelTrash, vec3(x, y, z));
		
		
	
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelTrash.m);
		render();
	}

	void Mesh::init(const char* file_name, const char* textureName) {

		this->modelData = load_mesh(file_name);
		this->loc1 = glGetAttribLocation(shaderProgram, "vertex_position");
		this->loc2 = glGetAttribLocation(shaderProgram, "vertex_normal");
		this->loc3 = glGetAttribLocation(shaderProgram, "vertex_texture");
		glGenBuffers(1, &vertexBufferP);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferP);
		glBufferData(GL_ARRAY_BUFFER, modelData.mPointCount * sizeof(vec3), &modelData.mVertices[0], GL_STATIC_DRAW);
		glGenBuffers(1, &vertexBufferN);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferN);
		glBufferData(GL_ARRAY_BUFFER, modelData.mPointCount * sizeof(vec3), &modelData.mNormals[0], GL_STATIC_DRAW);

		//	This is for texture coordinates which you don't currently need, so I have commented it out
		//	unsigned int vt_vbo = 0;
		//	glGenBuffers (1, &vt_vbo);
		//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
		//	glBufferData (GL_ARRAY_BUFFER, monkey_head_data.mTextureCoords * sizeof (vec2), &monkey_head_data.mTextureCoords[0], GL_STATIC_DRAW);

		glBindVertexArray(vao);
		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferP);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferN);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//Texture stuff-------------------------------------------------------
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int widthImg, heightImg, nrChannels;
		unsigned char* dataIMG = stbi_load(textureName, &widthImg, &heightImg, &nrChannels, 0);
		if (dataIMG)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, dataIMG);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(dataIMG);
		glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		//	This is for texture coordinates which you don't currently need, so I have commented it out
		//	glEnableVertexAttribArray (loc3);
		//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
		//	glVertexAttribPointer (loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	}

	void Mesh::setShaders(GLuint programID) {
		this->shaderProgram = programID;
	}

	void Mesh::setData() {
		bind();
		glBufferData(GL_ARRAY_BUFFER, modelData.mPointCount * sizeof(vec3), &modelData.mVertices[0], GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, modelData.mPointCount * sizeof(vec3), &modelData.mNormals[0], GL_STATIC_DRAW);
	}

	void Mesh::render() {
		glUseProgram(this->shaderProgram);
		bind();
		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferP);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferN);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(loc3);
		glBindTexture(GL_TEXTURE_2D, texture);
		glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glDrawArrays(GL_TRIANGLES, 0, modelData.mPointCount);

	}

	void Mesh::cleanup() {
		glDeleteBuffers(1, &vertexBufferP);
		glDeleteBuffers(1, &vertexBufferN);
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(shaderProgram);
	}
