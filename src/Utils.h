#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <future>
#include <glad/glad.h>
#include <thread>

#include "util/Logger.h"

using namespace std::chrono_literals;

std::mutex sortMutex;
std::mutex updateFreqMutex;
std::mutex dataSizeMutex;
int numUpdatesPerSec = 1;
int dataSize = 100;

struct VisualizationRectangle
{
public:
  VisualizationRectangle()
  {
    // Initializing VAO, (initilize 1 vao then bind)
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Initializing VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Initializing EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indicies, GL_STATIC_DRAW);

    // Unbinds currently bound vertex array
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

public:
  // Vertex array object
  uint32_t vao;
  // Vertex buffer object
  uint32_t vbo;
  // Element buffer object
  uint32_t ebo;

private:
  inline static float vertexData[] = {
    0.0f, 1.0f, 0.0f, //
    0.0f, 0.0f, 0.0f, //
    1.0f, 1.0f, 0.0f, //
    1.0f, 0.0f, 0.0f  //
  };

  // Order to run the vertecies (both in winding order)
  inline static uint32_t indicies[] = {
    0, 1, 2, //
    2, 1, 3  //
  };
};

struct SortableRenderData
{
public:
  // Overloading operator section
  bool operator<(const SortableRenderData& srd) { return data < srd.data; }

  bool operator>(const SortableRenderData& srd) { return data > srd.data; }

  bool operator<=(const SortableRenderData& srd) { return data <= srd.data; }

  bool operator>=(const SortableRenderData& srd) { return data >= srd.data; }

  friend std::ostream& operator<<(std::ostream& os, const SortableRenderData& srd);

public:
  float data;

  bool selected;

  bool high;

  bool low;
};

std::ostream& operator<<(std::ostream& os, const SortableRenderData& srd)
{
  os << srd.data;
  return os;
}

#endif
