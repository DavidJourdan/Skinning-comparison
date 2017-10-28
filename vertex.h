#pragma once

class Vertex {
public:
	inline Vertex(float x, float y, float z) {
		m_x = x;
		m_y = y;
		m_z = z;
	}
	inline Vertex() {m_x = m_y = m_z = 0.0;}

	~Vertex() {}
private:
	float m_x;
	float m_y;
	float m_z;
};