#pragma once

class Vertex {
public:
	inline Vertex(float x, float y, float z) {
		m_p[0] = x;
		m_p[1] = y;
		m_p[2] = z;
	}
	inline Vertex() {m_p[0] = m_p[1] = m_p[2] = 0.0;}

	~Vertex() {}
private:
	float m_p[3];
};