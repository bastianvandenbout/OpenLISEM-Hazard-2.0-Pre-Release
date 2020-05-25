#ifndef GLVERTEX_H
#define GLVERTEX_H

#include "QObject"
#include "sph_vector3.h"
#include "sph_vector2.h"

class Vertex
{
public:
  // Constructors
  Vertex();
  Vertex(const SPHVector3 &position);
  Vertex(const SPHVector3 &position, const SPHVector3 &color);
  Vertex(const SPHVector3 &position, const SPHVector3 &color, const SPHVector3 &normal);

  // Accessors / Mutators
  const SPHVector3& position() const;
  const SPHVector3& color() const;
  const SPHVector3& normal() const;
  void setPosition(const SPHVector3& position);
  void setColor(const SPHVector3& color);
  void setNormal(const SPHVector3& normal);

  // OpenGL Helpers
  static const int PositionTupleSize = 3;
  static const int ColorTupleSize = 3;
  static const int NormalTupleSize = 3;
  static const int UVTupleSize = 2;
  static const int TangentTupleSize = 3;
  static const int BiTangentTupleSize = 3;

  static int positionOffset();
  static int colorOffset();
  static int normalOffset();
  static int uvOffset();
  static int tangentOffset();
  static int bitangentOffset();

  static int stride();


  SPHVector3 m_position;
  SPHVector2 m_UV;
  SPHVector3 m_normal;
  SPHVector3 m_tangent;
  SPHVector3 m_bitangent;
  SPHVector3 m_color;

private:


};


// Constructors
inline Vertex::Vertex() {}
inline Vertex::Vertex(const SPHVector3 &position) : m_position(position) {}
inline Vertex::Vertex(const SPHVector3 &position, const SPHVector3 &color) : m_position(position), m_color(color) {}
inline Vertex::Vertex(const SPHVector3 &position, const SPHVector3 &color, const SPHVector3 &normal) : m_position(position), m_color(color), m_normal(normal) {}

// Accessors / Mutators
inline const SPHVector3& Vertex::position() const { return m_position; }
inline const SPHVector3& Vertex::color() const { return m_color; }
inline const SPHVector3& Vertex::normal() const { return m_normal; }
void inline Vertex::setPosition(const SPHVector3& position) { m_position = position; }
void inline Vertex::setColor(const SPHVector3& color) { m_color = color; }
void inline Vertex::setNormal(const SPHVector3& normal) { m_normal = normal; }

// OpenGL Helpers
inline int Vertex::positionOffset() { return offsetof(Vertex, m_position); }
inline int Vertex::colorOffset() { return offsetof(Vertex, m_color); }
inline int Vertex::normalOffset() { return offsetof(Vertex, m_normal); }
inline int Vertex::uvOffset() { return offsetof(Vertex, m_UV); }
inline int Vertex::tangentOffset() { return offsetof(Vertex, m_tangent); }
inline int Vertex::bitangentOffset() { return offsetof(Vertex, m_bitangent); }
inline int Vertex::stride() { return sizeof(Vertex); }

#endif // GLVERTEX_H
