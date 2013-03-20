// ShaderManager.h

#ifndef SHADERMANAGER_H_INCLUDED
#define SHADERMANAGER_H_INCLUDED

class RenderSystem;

class ShaderManager {
public:
	ShaderManager(RenderSystem *render_system);
	~ShaderManager();

	Shader *Create(const String &vertex_shader,const String &pixel_shader);
	void Destroy(Shader *shader);

private:
	typedef std::map<std::pair<String,String>,Shader*> ShaderMap;
	RenderSystem *m_render_system;
	Allocator<Shader> m_allocator;
	ShaderMap m_shaders;
};

#endif // SHADERMANAGER_H_INCLUDED
