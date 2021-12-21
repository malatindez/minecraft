#ifndef DP_RECT_PACK_H
#define DP_RECT_PACK_H

#include <cassert>
#include <cstddef>
#include <vector>

#define DP_RECT_PACK_VERSION_MAJOR 1
#define DP_RECT_PACK_VERSION_MINOR 1
#define DP_RECT_PACK_VERSION_PATCH 3

namespace utils {

struct InsertStatus {
  enum Type {
    ok,
    negativeSize,
    zeroSize,

    rectTooBig
  };
};

template <typename GeomT = int> class RectPacker {
public:
  struct Spacing {
    GeomT x;
    GeomT y;

    explicit Spacing(GeomT spacing) : x(spacing), y(spacing) {}

    Spacing(GeomT x, GeomT y) : x(x), y(y) {}
  };

  struct Padding {
    GeomT top;
    GeomT bottom;
    GeomT left;
    GeomT right;

    explicit Padding(GeomT padding)
        : top(padding), bottom(padding), left(padding), right(padding) {}

    Padding(GeomT top, GeomT bottom, GeomT left, GeomT right)
        : top(top), bottom(bottom), left(left), right(right) {}
  };

  struct Position {
    GeomT x;
    GeomT y;

    Position() : x(), y() {}

    Position(GeomT x, GeomT y) : x(x), y(y) {}
  };

  struct InsertResult {
    InsertStatus::Type status;

    Position pos;

    std::size_t pageIndex;
  };

  RectPacker(GeomT maxPageWidth, GeomT maxPageHeight,
             const Spacing &rectsSpacing = Spacing(0),
             const Padding &pagePadding = Padding(0))
      : ctx(maxPageWidth, maxPageHeight, rectsSpacing, pagePadding), pages(1) {}

  std::size_t getNumPages() const { return pages.size(); }

  void getPageSize(std::size_t pageIndex, GeomT &width, GeomT &height) const {
    const Size size = pages[pageIndex].getSize(ctx);
    width = size.w;
    height = size.h;
  }

  InsertResult insert(GeomT width, GeomT height);

private:
  struct Size {
    GeomT w;
    GeomT h;

    Size(GeomT w, GeomT h) : w(w), h(h) {}
  };

  struct Context;
  class Page {
  public:
    Page() : nodes(), rootSize(0, 0), growDownRootBottomIdx(0) {}

    Size getSize(const Context &ctx) const {
      return Size(ctx.padding.left + rootSize.w + ctx.padding.right,
                  ctx.padding.top + rootSize.h + ctx.padding.bottom);
    }

    bool insert(Context &ctx, const Size &rect, Position &pos);

  private:
    struct Node {
      Position pos;
      Size size;

      Node(GeomT x, GeomT y, GeomT w, GeomT h) : pos(x, y), size(w, h) {}
    };

    std::vector<Node> nodes;
    Size rootSize;

    std::size_t growDownRootBottomIdx;

    bool tryInsert(Context &ctx, const Size &rect, Position &pos);
    bool findNode(const Size &rect, std::size_t &nodeIdx, Position &pos) const;
    void subdivideNode(Context &ctx, std::size_t nodeIdx, const Size &rect);
    bool tryGrow(Context &ctx, const Size &rect, Position &pos);
    void growDown(Context &ctx, const Size &rect, Position &pos);
    void growRight(Context &ctx, const Size &rect, Position &pos);
  };

  struct Context {
    Size maxSize;
    Spacing spacing;
    Padding padding;

    Context(GeomT maxPageWidth, GeomT maxPageHeight,
            const Spacing &rectsSpacing, const Padding &pagePadding);

    static void subtractPadding(GeomT &padding, GeomT &size);
  };

  Context ctx;
  std::vector<Page> pages;
};

template <typename GeomT>
typename RectPacker<GeomT>::InsertResult
RectPacker<GeomT>::insert(GeomT width, GeomT height) {
  InsertResult result;

  if (width < 0 || height < 0) {
    result.status = InsertStatus::negativeSize;
    return result;
  }

  if (width == 0 || height == 0) {
    result.status = InsertStatus::zeroSize;
    return result;
  }

  if (width > ctx.maxSize.w || height > ctx.maxSize.h) {
    result.status = InsertStatus::rectTooBig;
    return result;
  }

  const Size rect(width, height);

  for (std::size_t i = 0; i < pages.size(); ++i)
    if (pages[i].insert(ctx, rect, result.pos)) {
      result.status = InsertStatus::ok;
      result.pageIndex = i;
      return result;
    }

  pages.push_back(Page());
  Page &page = pages.back();
  page.insert(ctx, rect, result.pos);
  result.status = InsertStatus::ok;
  result.pageIndex = pages.size() - 1;

  return result;
}

template <typename GeomT>
bool RectPacker<GeomT>::Page::insert(Context &ctx, const Size &rect,
                                     Position &pos) {
  assert(rect.w > 0);
  assert(rect.w <= ctx.maxSize.w);
  assert(rect.h > 0);
  assert(rect.h <= ctx.maxSize.h);

  if (rootSize.w == 0) {
    rootSize = rect;
    pos.x = ctx.padding.left;
    pos.y = ctx.padding.top;

    return true;
  }

  return tryInsert(ctx, rect, pos) || tryGrow(ctx, rect, pos);
}

template <typename GeomT>
bool RectPacker<GeomT>::Page::tryInsert(Context &ctx, const Size &rect,
                                        Position &pos) {
  std::size_t nodeIdx;
  if (findNode(rect, nodeIdx, pos)) {
    subdivideNode(ctx, nodeIdx, rect);
    return true;
  }

  return false;
}

template <typename GeomT>
bool RectPacker<GeomT>::Page::findNode(const Size &rect, std::size_t &nodeIdx,
                                       Position &pos) const {
  for (nodeIdx = 0; nodeIdx < nodes.size(); ++nodeIdx) {
    const Node &node = nodes[nodeIdx];
    if (rect.w <= node.size.w && rect.h <= node.size.h) {
      pos = node.pos;
      return true;
    }
  }

  return false;
}

template <typename GeomT>
void RectPacker<GeomT>::Page::subdivideNode(Context &ctx, std::size_t nodeIdx,
                                            const Size &rect) {
  assert(nodeIdx < nodes.size());

  Node &node = nodes[nodeIdx];

  assert(node.size.w >= rect.w);
  const GeomT rightW = node.size.w - rect.w;
  const bool hasSpaceRight = rightW > ctx.spacing.x;

  assert(node.size.h >= rect.h);
  const GeomT bottomH = node.size.h - rect.h;
  const bool hasSpaceBelow = bottomH > ctx.spacing.y;

  if (hasSpaceRight) {
    const GeomT bottomX = node.pos.x;
    const GeomT bottomW = node.size.w;

    node.pos.x += rect.w + ctx.spacing.x;
    node.size.w = rightW - ctx.spacing.x;
    node.size.h = rect.h;

    if (hasSpaceBelow) {
      nodes.insert(nodes.begin() + nodeIdx + 1,
                   Node(bottomX, node.pos.y + rect.h + ctx.spacing.y, bottomW,
                        bottomH - ctx.spacing.y));

      if (nodeIdx <= growDownRootBottomIdx)
        ++growDownRootBottomIdx;
    }
  } else if (hasSpaceBelow) {
    node.pos.y += rect.h + ctx.spacing.y;
    node.size.h = bottomH - ctx.spacing.y;
  } else {
    nodes.erase(nodes.begin() + nodeIdx);
    if (nodeIdx < growDownRootBottomIdx)
      --growDownRootBottomIdx;
  }
}

template <typename GeomT>
bool RectPacker<GeomT>::Page::tryGrow(Context &ctx, const Size &rect,
                                      Position &pos) {
  assert(ctx.maxSize.w >= rootSize.w);
  const GeomT freeW = ctx.maxSize.w - rootSize.w;
  assert(ctx.maxSize.h >= rootSize.h);
  const GeomT freeH = ctx.maxSize.h - rootSize.h;

  const bool canGrowDown = (freeH >= rect.h && freeH - rect.h >= ctx.spacing.y);
  const bool mustGrowDown =
      (canGrowDown && freeW >= ctx.spacing.x &&
       (rootSize.w + ctx.spacing.x >= rootSize.h + rect.h + ctx.spacing.y));
  if (mustGrowDown) {
    growDown(ctx, rect, pos);
    return true;
  }

  const bool canGrowRight =
      (freeW >= rect.w && freeW - rect.w >= ctx.spacing.x);
  if (canGrowRight) {
    growRight(ctx, rect, pos);
    return true;
  }

  if (canGrowDown) {
    growDown(ctx, rect, pos);
    return true;
  }

  return false;
}

template <typename GeomT>
void RectPacker<GeomT>::Page::growDown(Context &ctx, const Size &rect,
                                       Position &pos) {
  assert(ctx.maxSize.h > rootSize.h);
  assert(ctx.maxSize.h - rootSize.h >= rect.h);
  assert(ctx.maxSize.h - rootSize.h - rect.h >= ctx.spacing.y);

  pos.x = ctx.padding.left;
  pos.y = ctx.padding.top + rootSize.h + ctx.spacing.y;

  if (rootSize.w < rect.w) {
    if (rect.w - rootSize.w > ctx.spacing.x) {
      nodes.insert(nodes.begin(),
                   Node(ctx.padding.left + rootSize.w + ctx.spacing.x,
                        ctx.padding.top, rect.w - rootSize.w - ctx.spacing.x,
                        rootSize.h));
      ++growDownRootBottomIdx;
    }

    rootSize.w = rect.w;
  } else if (rootSize.w - rect.w > ctx.spacing.x) {
    nodes.insert(nodes.begin() + growDownRootBottomIdx,
                 Node(pos.x + rect.w + ctx.spacing.x, pos.y,
                      rootSize.w - rect.w - ctx.spacing.x, rect.h));

    ++growDownRootBottomIdx;
  }

  rootSize.h += ctx.spacing.y + rect.h;
}

template <typename GeomT>
void RectPacker<GeomT>::Page::growRight(Context &ctx, const Size &rect,
                                        Position &pos) {
  assert(ctx.maxSize.w > rootSize.w);
  assert(ctx.maxSize.w - rootSize.w >= rect.w);
  assert(ctx.maxSize.w - rootSize.w - rect.w >= ctx.spacing.x);

  pos.x = ctx.padding.left + rootSize.w + ctx.spacing.x;
  pos.y = ctx.padding.top;

  if (rootSize.h < rect.h) {
    if (rect.h - rootSize.h > ctx.spacing.y)

      nodes.insert(nodes.end(),
                   Node(ctx.padding.left,
                        ctx.padding.top + rootSize.h + ctx.spacing.y,
                        rootSize.w, rect.h - rootSize.h - ctx.spacing.y));

    rootSize.h = rect.h;
  } else if (rootSize.h - rect.h > ctx.spacing.y) {
    nodes.insert(nodes.begin(),
                 Node(pos.x, pos.y + rect.h + ctx.spacing.y, rect.w,
                      rootSize.h - rect.h - ctx.spacing.y));
    ++growDownRootBottomIdx;
  }

  rootSize.w += ctx.spacing.x + rect.w;
}

template <typename GeomT>
RectPacker<GeomT>::Context::Context(GeomT maxPageWidth, GeomT maxPageHeight,
                                    const Spacing &rectsSpacing,
                                    const Padding &pagePadding)
    : maxSize(maxPageWidth, maxPageHeight), spacing(rectsSpacing),
      padding(pagePadding) {
  if (maxSize.w < 0)
    maxSize.w = 0;
  if (maxSize.h < 0)
    maxSize.h = 0;

  if (spacing.x < 0)
    spacing.x = 0;
  if (spacing.y < 0)
    spacing.y = 0;

  subtractPadding(padding.top, maxSize.h);
  subtractPadding(padding.bottom, maxSize.h);
  subtractPadding(padding.left, maxSize.w);
  subtractPadding(padding.right, maxSize.w);
}

template <typename GeomT>
void RectPacker<GeomT>::Context::subtractPadding(GeomT &padding, GeomT &size) {
  if (padding < 0)
    padding = 0;
  else if (padding < size)
    size -= padding;
  else {
    padding = size;
    size = 0;
  }
}

} // namespace utils

#endif
