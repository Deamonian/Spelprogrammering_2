#pragma once

namespace Gui{
	struct Vertex{
		Vector3 position;
		unsigned int color;
	};

	enum EWidgetType{WT_WIDGET, WT_BUTTON,};

	class Widget{
	public:
		Widget(EWidgetType type);
		EWidgetType GetType() const;
		float GetX() const;
		float GetY() const;
		float GetWidth() const;
		float GetHeight() const;

		void SetX(float x);
		void setY(float y);
		void SetWidth(float width);
		void setHeight(float height);

		bool IsVisible() const;
		bool IsEnabled() const;
		bool IsDirty() const;
		void Clean();

		virtual bool IsInside(int x, int y);

	protected:
		EWidgetType m_type;
		float m_x, m_y;
		float m_width, m_height;
		bool m_visible;
		bool m_enabled;
		bool m_dirty;
	};

	class Button : public Widget {
	public:
		Button();

		unsigned int GetColor() const;
		void SetColor(unsigned int color);
		String GetText() const;
		void SetText(const String &text);
		bool IsInside(int x, int y);


	private:
		unsigned int m_color;
		String m_text;
	};
};//namespace Gui

class GuiManager{
public:
	GuiManager(class RenderSystem *render_system, class MessageSystem *message_system);
	~GuiManager();

	bool Init(int width, int height, unsigned int max_vertices=1024);
	void Shut();
	void Update();
	void Render();

	void AttachWidget(Gui::Widget *widget);
	void DetachWidget(Gui::Widget *widget);

private:
	void OnMouseMove(IEvent *event);
	void OnMouseButton(IEvent *event);

	std::vector<Gui::Widget*> m_widgets;
	class RenderSystem* m_render_system;
	class MessageSystem* m_message_system;

	void FillButton(Gui::Widget *widget, Gui::Vertex *vertices, unsigned int size, unsigned int &pos);

	struct{
		Shader m_shader;
		Renderable m_renderable;
		unsigned int m_max_vertices;
		Gui::Vertex *m_vertices;
	} m_gui;
};