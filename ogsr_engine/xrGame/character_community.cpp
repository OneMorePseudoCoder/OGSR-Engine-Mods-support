//////////////////////////////////////////////////////////////////////////
// character_community.cpp:		структура представления группировки
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_community.h"

//////////////////////////////////////////////////////////////////////////
COMMUNITY_DATA::COMMUNITY_DATA(CHARACTER_COMMUNITY_INDEX idx, CHARACTER_COMMUNITY_ID idn, LPCSTR team_str)
{
    index = idx;
    id = idn;
    team = (u8)atoi(team_str);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_COMMUNITY::GOODWILL_TABLE CHARACTER_COMMUNITY::m_relation_table;
CHARACTER_COMMUNITY::SYMPATHY_TABLE CHARACTER_COMMUNITY::m_sympathy_table;

//////////////////////////////////////////////////////////////////////////
CHARACTER_COMMUNITY::CHARACTER_COMMUNITY() { m_current_index = NO_COMMUNITY_INDEX; }
CHARACTER_COMMUNITY::~CHARACTER_COMMUNITY() {}

void CHARACTER_COMMUNITY::set(CHARACTER_COMMUNITY_ID id) { m_current_index = IdToIndex(id); }

CHARACTER_COMMUNITY_ID CHARACTER_COMMUNITY::id() const { return IndexToId(m_current_index); }

u8 CHARACTER_COMMUNITY::team() const { return (*m_pItemDataVector)[m_current_index].team; }

void CHARACTER_COMMUNITY::InitIdToIndex()
{
    section_name = "game_relations";
    line_name = "communities";

    m_relation_table.set_table_params("communities_relations");
    m_sympathy_table.set_table_params("communities_sympathy", 1);
}

CHARACTER_GOODWILL CHARACTER_COMMUNITY::relation(CHARACTER_COMMUNITY_INDEX to) { return relation(m_current_index, to); }

CHARACTER_GOODWILL CHARACTER_COMMUNITY::relation(CHARACTER_COMMUNITY_INDEX from, CHARACTER_COMMUNITY_INDEX to)
{
    //SIMP: встретил ситуацию когда from == -1
    const bool cond1{from >= 0 && from < static_cast<int>(m_relation_table.table().size())};
    ASSERT_FMT_DBG(cond1, "!![%s] wrong first community index: [%d], m_relation_table size: [%u]", __FUNCTION__, from, m_relation_table.table().size());
    const bool cond2{to >= 0 && to < static_cast<int>(m_relation_table.table().size())};
    ASSERT_FMT_DBG(cond2, "!![%s] wrong second community index: [%d], m_relation_table size: [%u]", __FUNCTION__, to, m_relation_table.table().size());

    if (!cond1 || !cond2)
    {
        if (auto handler = Debug.get_crashhandler()) // выводит в лог луашный сетктрейс в данной ситуации
            handler(false);
        return 0;
    }

    return m_relation_table.table()[from][to];
}

void CHARACTER_COMMUNITY::set_relation(CHARACTER_COMMUNITY_INDEX from, CHARACTER_COMMUNITY_INDEX to, CHARACTER_GOODWILL goodwill)
{
    const bool cond1{from >= 0 && from < static_cast<int>(m_relation_table.table().size())};
    ASSERT_FMT_DBG(cond1, "!![%s] wrong first community index: [%d], m_relation_table size: [%u]", __FUNCTION__, from, m_relation_table.table().size());
    const bool cond2{to >= 0 && to < static_cast<int>(m_relation_table.table().size())};
    ASSERT_FMT_DBG(cond2, "!![%s] wrong second community index: [%d], m_relation_table size: [%u]", __FUNCTION__, to, m_relation_table.table().size());

    if (!cond1 || !cond2)
    {
        if (auto handler = Debug.get_crashhandler()) //выводит в лог луашный сетктрейс в данной ситуации
            handler(false);
        return;
    }

    VERIFY(goodwill != NO_GOODWILL);

    m_relation_table.table()[from][to] = goodwill;
}

float CHARACTER_COMMUNITY::sympathy(CHARACTER_COMMUNITY_INDEX comm)
{
    const bool cond{comm >= 0 && comm < static_cast<int>(m_sympathy_table.table().size())};
    ASSERT_FMT_DBG(cond, "!![%s] wrong community index: [%d], m_sympathy_table size: [%u]", __FUNCTION__, comm, m_sympathy_table.table().size());

    if (!cond)
        return 0.f;

    return m_sympathy_table.table()[comm][0];
}

void CHARACTER_COMMUNITY::DeleteIdToIndexData()
{
    m_relation_table.clear();
    m_sympathy_table.clear();
    inherited::DeleteIdToIndexData();
}