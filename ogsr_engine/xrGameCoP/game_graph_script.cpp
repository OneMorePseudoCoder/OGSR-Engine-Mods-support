////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_graph.h"
#include "ai_space.h"

using namespace luabind;

const CGameGraph* get_game_graph() { return (&ai().game_graph()); }

const CGameGraph::CHeader* get_header(const CGameGraph* self) { return (&self->header()); }

bool get_accessible1(const CGameGraph* self, const u32& vertex_id) { return (self->accessible(vertex_id)); }

void get_accessible2(const CGameGraph* self, const u32& vertex_id, bool value) { self->accessible(vertex_id, value); }

Fvector CVertex__level_point(const CGameGraph::CVertex* vertex)
{
    THROW(vertex);
    return (vertex->level_point());
}

Fvector CVertex__game_point(const CGameGraph::CVertex* vertex)
{
    THROW(vertex);
    return (vertex->game_point());
}

GameGraph::LEVEL_MAP const& get_levels(CGameGraph const* graph)
{
    THROW(graph);
    return graph->header().levels();
}

u32 CGameGraph__vertex_level_id(const CGameGraph* self, u32 vertex_id)
{
    THROW(self);
    return self->vertex(vertex_id)->level_id();
}

u32 CGameGraph__levels_count(const CGameGraph* self)
{
    THROW(self);
    return (u32)self->header().levels().size();
}

u32 CGameGraph__level_id(const CGameGraph* self, size_t index)
{
    THROW(self);
    const auto& levels = self->header().levels();
    return levels.key_at(index);
}

float CGameGraph__distance(const CGameGraph* self, u32 left_id, u32 right_id)
{
    THROW(self);

    const auto* left = self->vertex(left_id);
    const auto* right = self->vertex(right_id);

    return left->game_point().distance_to(right->game_point());
}

void CGameGraph::script_register(lua_State* L)
{
    module(L)[class_<GameGraph::LEVEL_MAP::value_type>("GameGraph__LEVEL_MAP__value_type")
                  .def_readonly("id", &GameGraph::LEVEL_MAP::value_type::first)
                  .def_readonly("level", &GameGraph::LEVEL_MAP::value_type::second),

              def("game_graph", &get_game_graph),

              class_<CGameGraph>("CGameGraph")
                  .def("accessible", &get_accessible1)
                  .def("accessible", &get_accessible2)
                  .def("valid_vertex_id", &CGameGraph::valid_vertex_id)
                  .def("vertex", &CGameGraph::vertex)
                  .def("vertex_id", &CGameGraph::vertex_id)
                  .def("levels", &get_levels, return_stl_iterator)
                  .def("vertex_level_id", &CGameGraph__vertex_level_id)
                  .def("levels_count", &CGameGraph__levels_count)
                  .def("level_id", &CGameGraph__level_id)
                  .def("distance", &CGameGraph__distance),

              class_<CVertex>("GameGraph__CVertex")
                  .def("level_point", &CVertex__level_point)
                  .def("game_point", &CVertex__game_point)
                  .def("level_id", &CVertex::level_id)
                  .def("level_vertex_id", &CVertex::level_vertex_id),
              def(
                  "gg_vertex_level_id", +[](u32 vertex_id) { return ai().game_graph().vertex(vertex_id)->level_id(); }),
              def("gg_level_id",
                  [](_LEVEL_ID idx) {
                      const GameGraph::LEVEL_MAP& levels = ai().game_graph().header().levels();
                      return (levels.begin() + idx)->second.id();
                  }),
              def(
                  "gg_levels_count", +[]() { return ai().game_graph().header().level_count(); }),
              def(
                  "gg_distance", +[](u32 vid1, u32 vid2) {
                      const auto& game_graph = ai().game_graph();
                      const auto p1 = game_graph.vertex(vid1)->game_point();
                      const auto p2 = game_graph.vertex(vid2)->game_point();
                      return p1.distance_to(p2);
                  })];
}
