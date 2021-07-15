#ifndef UTILS_H
#define UTILS_H

struct SortableRenderData
{
  float data;

  bool operator<(const SortableRenderData& srd) { return data < srd.data; }

  bool operator>(const SortableRenderData& srd) { return data > srd.data; }

  bool operator<=(const SortableRenderData& srd) { return data <= srd.data; }

  bool operator>=(const SortableRenderData& srd) { return data >= srd.data; }

  friend std::ostream& operator<<(std::ostream& os, const SortableRenderData& srd);
};

std::ostream& operator<<(std::ostream& os, const SortableRenderData& srd)
{
  os << srd.data;
  return os;
}

#endif