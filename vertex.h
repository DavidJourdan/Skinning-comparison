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

	inline float& operator[] (int i) {
		return m_p[i];
	}

	inline const float& operator[] (int i) const {
		return m_p[i];
	}

	inline Vertex& operator= (const Vertex & p) {
		m_p[0] = p[0];
		m_p[1] = p[1];
		m_p[2] = p[2];
		return (*this);
	}

	inline Vertex& operator+= (const Vertex & p) {
		m_p[0] += p[0];
		m_p[1] += p[1];
		m_p[2] += p[2];
		return (*this);
	}

	inline Vertex operator+ (const Vertex & p) const {
		Vertex v;
		v[0] = m_p[0] + p[0];
		v[1] = m_p[1] + p[1];
		v[2] = m_p[2] + p[2];
		return v;
	}


	inline Vertex operator- (const Vertex & p) const {
		Vertex v;
		v[0] = m_p[0] - p[0];
		v[1] = m_p[1] - p[1];
		v[2] = m_p[2] - p[2];
		return v;
	}

	inline Vertex operator/ (float s) const {
		Vertex v;
		v[0] = m_p[0] / s;
		v[1] = m_p[1] / s;
		v[2] = m_p[2] / s;
		return v;
	}

	inline Vertex operator* (float s) const {
		Vertex v;
		v[0] = m_p[0] * s;
		v[1] = m_p[1] * s;
		v[2] = m_p[2] * s;
		return v;
	}

	inline Vertex cross (const Vertex & v) {
		Vertex res;
		res[0] = m_p[1] * v[2] - m_p[2] * v[1];
		res[1] = m_p[2] * v[0] - m_p[0] * v[2];
		res[2] = m_p[0] * v[1] - m_p[1] * v[0];
		return res;
	}

	inline float len() {
		return m_p[0]*m_p[0] + m_p[1]*m_p[1] + m_p[2]*m_p[2];
	}
private:
	float m_p[3];
};
