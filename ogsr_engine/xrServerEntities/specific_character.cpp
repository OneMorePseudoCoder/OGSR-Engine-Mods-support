#include "stdafx.h"
#include "specific_character.h"
#include "PhraseDialog.h"
#include "string_table.h"

SSpecificCharacterData::SSpecificCharacterData()
{
    m_sGameName.clear();
    m_sBioText = nullptr;
    m_sVisual.clear();
    m_sSupplySpawn.clear();
    m_sNpcConfigSect.clear();

    m_StartDialog = nullptr;
    m_ActorDialogs.clear();

    m_Rank = NO_RANK;
    m_Reputation = NO_REPUTATION;

    m_bNoRandom = false;
    m_bDefaultForCommunity = false;
    m_fPanic_threshold = 0.0f;
    m_fHitProbabilityFactor = 1.f;
    m_crouch_type = 0;
    m_upgrade_mechanic = false;

    first_visual = -1;
    last_visual = -1;
}

SSpecificCharacterData::~SSpecificCharacterData() {}

CSpecificCharacter::CSpecificCharacter() { m_OwnId = nullptr; }

CSpecificCharacter::~CSpecificCharacter() {}

void CSpecificCharacter::InitXmlIdToIndex()
{
    if (!id_to_index::tag_name)
        id_to_index::tag_name = "specific_character";
    if (!id_to_index::file_str)
        id_to_index::file_str = pSettings->r_string("profiles", "specific_characters_files");
}

void CSpecificCharacter::Load(shared_str id)
{
    R_ASSERT(id.size());
    m_OwnId = id;
    inherited_shared::load_shared(m_OwnId, nullptr);
}

void CSpecificCharacter::load_shared(LPCSTR)
{
    const ITEM_DATA& item_data = *id_to_index::GetById(m_OwnId);

    CUIXml* pXML = item_data._xml;

    pXML->SetLocalRoot(pXML->GetRoot());

    XML_NODE* item_node = pXML->NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
    R_ASSERT3(item_node, "specific_character id=", *item_data.id);

    pXML->SetLocalRoot(item_node);

    int norandom = pXML->ReadAttribInt(item_node, "no_random", 0);
    if (1 == norandom)
        data()->m_bNoRandom = true;
    else
        data()->m_bNoRandom = false;

    int team_default = pXML->ReadAttribInt(item_node, "team_default", 0);
    if (1 == team_default)
        data()->m_bDefaultForCommunity = true;
    else
        data()->m_bDefaultForCommunity = false;

    R_ASSERT3(!(data()->m_bNoRandom && data()->m_bDefaultForCommunity), "cannot set 'no_random' and 'team_default' flags simultaneously, profile id", *shared_str(item_data.id));

    LPCSTR start_dialog = pXML->Read("start_dialog", 0, nullptr);
    if (start_dialog)
    {
        data()->m_StartDialog = start_dialog;
    }
    else
        data()->m_StartDialog = nullptr;

    int dialogs_num = pXML->GetNodesNum(pXML->GetLocalRoot(), "actor_dialog");
    data()->m_ActorDialogs.clear();
    for (int i = 0; i < dialogs_num; ++i)
    {
        shared_str dialog_name = pXML->Read(pXML->GetLocalRoot(), "actor_dialog", i, "");
        data()->m_ActorDialogs.push_back(dialog_name);
    }

    // игровое имя персонажа
    data()->m_sGameName = pXML->Read("name", 0, "");
    data()->m_sBioText = pXML->Read("bio", 0, "");

    data()->m_fPanic_threshold = pXML->ReadFlt("panic_threshold", 0, 0.f);
    data()->m_fHitProbabilityFactor = pXML->ReadFlt("hit_probability_factor", 0, 1.f);
    data()->m_crouch_type = pXML->ReadInt("crouch_type", 0, 0);
    data()->m_upgrade_mechanic = (pXML->ReadInt("mechanic_mode", 0, 0) == 1);

    data()->m_critical_wound_weights = pXML->Read("critical_wound_weights", 0, "1");

    data()->m_sVisual = pXML->Read("visual", 0, "");

    data()->m_bForceDisabledRandomIcons = pXML->ReadAttribInt("icon", 0, "force_disabled_random", 0) == 1;

    if (data()->m_sVisual.back() != '_' || data()->m_bForceDisabledRandomIcons)
        data()->m_icon_name = pXML->Read("icon", 0, "ui_npc_u_barman");

    data()->m_sSupplySpawn = pXML->Read("supplies", 0, "");

    if (!data()->m_sSupplySpawn.empty())
    {
        xr_string& str = data()->m_sSupplySpawn;
        xr_string::size_type pos = str.find("\\n");

        while (xr_string::npos != pos)
        {
            str.replace(pos, 2, "\n");
            pos = str.find("\\n", pos + 1);
        }
    }

    data()->m_sNpcConfigSect = pXML->Read("npc_config", 0, "");
    data()->m_sound_voice_prefix = pXML->Read("snd_config", 0, "");

    data()->m_terrain_sect = pXML->Read("terrain_sect", 0, "");

    data()->m_Classes.clear();
    int classes_num = pXML->GetNodesNum(pXML->GetLocalRoot(), "class");
    for (int i = 0; i < classes_num; i++)
    {
        LPCSTR char_class = pXML->Read("class", 0, "");
        if (char_class)
        {
            char* buf_str = xr_strdup(char_class);
            xr_strlwr(buf_str);
            data()->m_Classes.push_back(buf_str);
            xr_free(buf_str);
        }
    }

    LPCSTR team = pXML->Read("community", 0, nullptr);
    R_ASSERT3(team != nullptr, "'community' field not fulfiled for specific character", *m_OwnId);

    char* buf_str = xr_strdup(team);
    xr_strlwr(buf_str);
    data()->m_Community.set(buf_str);
    xr_free(buf_str);

    if (data()->m_Community.index() == NO_COMMUNITY_INDEX)
        Debug.fatal(DEBUG_INFO, "wrong 'community' '%s' in specific character %s ", team, *m_OwnId);

    data()->m_Rank = pXML->ReadInt("rank", 0, NO_RANK);
    R_ASSERT3(data()->m_Rank != NO_RANK, "'rank' field not fulfiled for specific character", *m_OwnId);
    data()->m_Reputation = pXML->ReadInt("reputation", 0, NO_REPUTATION);
    R_ASSERT3(data()->m_Reputation != NO_REPUTATION, "'reputation' field not fulfiled for specific character", *m_OwnId);

    if (pXML->NavigateToNode(pXML->GetLocalRoot(), "money", 0))
    {
        MoneyDef().min_money = pXML->ReadAttribInt("money", 0, "min");
        MoneyDef().max_money = pXML->ReadAttribInt("money", 0, "max");
        MoneyDef().inf_money = !!pXML->ReadAttribInt("money", 0, "infinitive");
        MoneyDef().max_money = _max(MoneyDef().max_money, MoneyDef().min_money); // :)
    }
    else
    {
        MoneyDef().min_money = 0;
        MoneyDef().max_money = 0;
        MoneyDef().inf_money = false;
    }
}

LPCSTR CSpecificCharacter::Name() const { return data()->m_sGameName.c_str(); }

shared_str CSpecificCharacter::Bio() const { return data()->m_sBioText; }

const CHARACTER_COMMUNITY& CSpecificCharacter::Community() const { return data()->m_Community; }

LPCSTR CSpecificCharacter::SupplySpawn() const { return data()->m_sSupplySpawn.c_str(); }

LPCSTR CSpecificCharacter::NpcConfigSect() const { return data()->m_sNpcConfigSect.c_str(); }

LPCSTR CSpecificCharacter::sound_voice_prefix() const { return data()->m_sound_voice_prefix.c_str(); }

float CSpecificCharacter::panic_threshold() const { return data()->m_fPanic_threshold; }

float CSpecificCharacter::hit_probability_factor() const { return data()->m_fHitProbabilityFactor; }

int CSpecificCharacter::crouch_type() const { return data()->m_crouch_type; }

bool CSpecificCharacter::upgrade_mechanic() const { return data()->m_upgrade_mechanic; }

LPCSTR CSpecificCharacter::critical_wound_weights() const { return data()->m_critical_wound_weights.c_str(); }

shared_str CSpecificCharacter::terrain_sect() const { return data()->m_terrain_sect; }

CHARACTER_RANK_VALUE CSpecificCharacter::Rank() const { return data()->m_Rank; }

CHARACTER_REPUTATION_VALUE CSpecificCharacter::Reputation() const { return data()->m_Reputation; }

LPCSTR CSpecificCharacter::Visual()
{
    const xr_string& base_visual = data()->m_sVisual;

    if (base_visual.empty())
        return "";

    if (base_visual.back() == '_')
    {
        SetRandomRange();

        int rnd_vis = data()->last_visual;

        if (data()->first_visual < data()->last_visual)
            rnd_vis = ::Random.randI(data()->first_visual, data()->last_visual);

        data()->m_cached_visual.clear();
        data()->m_cached_visual.reserve(base_visual.size() + 8);
        data()->m_cached_visual.append(base_visual);
        data()->m_cached_visual.append(std::to_string(rnd_vis));

        // Icon
        if (!data()->m_bForceDisabledRandomIcons)
        {
            const xr_string& full = data()->m_cached_visual;

            size_t pos = full.find_last_of("\\/");
            const char* file_name = (pos != xr_string::npos) ? full.c_str() + pos + 1 : full.c_str();

            xr_string icon;
            icon.reserve(8 + strlen(file_name));
            icon.append("ui_npc_");
            icon.append(file_name);

            data()->m_icon_name = icon.c_str();
        }

        return data()->m_cached_visual.c_str();
    }

    if (base_visual.back() == '*')
    {
        FS_FileSet fset;
        FS.file_list(fset, "$game_meshes$", FS_ListFiles, base_visual.c_str());

        if (fset.empty())
        {
            Msg("[CSpecificCharacter::Visual]: File list is empty!");
            return base_visual.c_str();
        }

        int rnd_index = 0;

        if (fset.size() > 1)
            rnd_index = ::Random.randI(0, (int)fset.size() - 1);

        auto it = fset.begin();
        std::advance(it, rnd_index);

        data()->m_cached_visual = it->name;

        if (!data()->m_bForceDisabledRandomIcons)
        {
            const xr_string& full = data()->m_cached_visual;

            size_t pos = full.find_last_of("\\/");
            const char* file_name = (pos != xr_string::npos) ? full.c_str() + pos + 1 : full.c_str();

            xr_string icon;
            icon.reserve(8 + strlen(file_name));
            icon.append("ui_npc_");
            icon.append(file_name);

            data()->m_icon_name = icon.c_str();
        }

        return data()->m_cached_visual.c_str();
    }

    data()->m_cached_visual = base_visual;
    return data()->m_cached_visual.c_str();
}

void CSpecificCharacter::SetRandomRange()
{
    const xr_string& base_visual = data()->m_sVisual;

    if (base_visual.empty())
    {
        data()->first_visual = 1;
        data()->last_visual = 1;
        return;
    }

    string_path full_mask;
    xr_strcpy(full_mask, base_visual.c_str());
    xr_strcat(full_mask, "*");

    FS_FileSet fset;
    FS.file_list(fset, "$game_meshes$", FS_ListFiles, full_mask);

    int min_num = INT_MAX;
    int max_num = INT_MIN;
    bool found = false;

    for (const auto& file : fset)
    {
        const xr_string& name = file.name;

        if (name.find(base_visual) != 0)
            continue;

        const char* suffix = name.c_str() + base_visual.length();

        try
        {
            int num = std::stoi(suffix);

            min_num = std::min(min_num, num);
            max_num = std::max(max_num, num);
            found = true;
        }
        catch (...)
        {
#ifdef DEBUG
            Msg("[CSpecificCharacter::SetRandomRange]: Invalid numeric suffix: %s", name.c_str());
#endif
        }
    }

    if (!found)
    {
#ifdef DEBUG
        Msg("[CSpecificCharacter::SetRandomRange]: No numbered models found for %s", base_visual.c_str());
#endif
        min_num = 1;
        max_num = 1;
    }

    data()->first_visual = min_num;
    data()->last_visual = max_num;
}