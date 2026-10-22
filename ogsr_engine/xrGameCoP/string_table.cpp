#include "stdafx.h"
#include "string_table.h"

#include "ui/xrUIXmlParser.h"
#include "xr_level_controller.h"
#include "MainMenu.h"

STRING_TABLE_DATA* CStringTable::pData{};
BOOL CStringTable::m_bWriteErrorsToLog{};

const xr_token* GetLanguagesToken();

CStringTable::CStringTable() { Init(); }

void CStringTable::Destroy() { xr_delete(pData); }
void CStringTable::rescan()
{
    if (nullptr != pData)
        return;
    Destroy();
    Init();
}

void CStringTable::Init()
{
    if (pData)
        return;

    pData = xr_new<STRING_TABLE_DATA>();

    // имя языка, если не задано (nullptr), то первый <text> в <string> в XML
    SetLanguage();

    auto load_xmls = [&](const bool nf) {
        FS_FileSet fset;
        string_path files_mask;

        if (nf)
            strcpy_s(files_mask, "text\\*.xml");
        else
            xr_sprintf(files_mask, "text\\%s\\*.xml", pData->m_sLanguage.c_str());

        FS.file_list(fset, fsgame::game_configs, nf ? (FS_ListFiles | FS_RootOnly) : FS_ListFiles, files_mask);
        for (const auto& file : fset)
        {
            string128 fn, ext;
            _splitpath(file.name.c_str(), nullptr, nullptr, fn, ext);
            xr_strcat(fn, ext);

            Load(fn, nf);
        }

        Msg("--[StringTable]: loaded [%u] files from path [%s]", fset.size(), files_mask);
    };

    load_xmls(false); // old-style
    load_xmls(true); // new

    ReparseKeyBindings();
}

void CStringTable::Load(LPCSTR xml_file_full, const bool nf)
{
    string_path _s;
    if (nf)
        strcpy_s(_s, STRING_TABLE_PATH);
    else
        xr_strconcat(_s, "text\\", pData->m_sLanguage.c_str());

    CUIXml uiXml;
    uiXml.Load(CONFIG_PATH, _s, xml_file_full);

    // общий список всех записей таблицы в файле
    int string_num = uiXml.GetNodesNum(uiXml.GetRoot(), "string");

    for (int i = 0; i < string_num; ++i)
    {
        LPCSTR string_name = uiXml.ReadAttrib(uiXml.GetRoot(), "string", i, "id", nullptr);

        if (m_bWriteErrorsToLog && pData->m_StringTable.contains(string_name))
            Msg("!![string table] duplicate string table id [%s]", string_name);

        string128 node;
        xr_strconcat(node, "string:", nf ? pData->m_sLanguage.c_str() : "text");
        LPCSTR string_text = uiXml.Read(uiXml.GetRoot(), node, i, nullptr);

        if (nf)
            ASSERT_FMT(string_text, "[string table] no attribute [%s] in node [%s]", node, string_name);
        else if (m_bWriteErrorsToLog && !string_text)
            Msg("!![string table] [%s] no translation in [%s]", string_name, pData->m_sLanguage.c_str());

        STRING_VALUE str_val = ParseLine(string_text, string_name, true);

        pData->m_StringTable[string_name] = str_val;
    }
}

void CStringTable::ReloadLanguage()
{
    if (!strcmp(GetLanguagesToken()->name, pData->m_sLanguage.c_str()))
        return;

    Destroy();
    Init();

    if (g_pGamePersistent && MainMenu() && MainMenu()->IsActive())
    {
        MainMenu()->Activate(FALSE);
        MainMenu()->Activate(TRUE);
    }
}

void CStringTable::SetLanguage() { pData->m_sLanguage = GetLanguagesToken()->name; }

void CStringTable::ReparseKeyBindings()
{
    if (!pData)
        return;

    for (const auto& [key, val] : pData->m_string_key_binding)
        pData->m_StringTable[key] = ParseLine(val.c_str(), key.c_str(), false);
}

STRING_VALUE CStringTable::ParseLine(LPCSTR str, LPCSTR skey, bool bFirst)
{
    //	LPCSTR str = "1 $$action_left$$ 2 $$action_right$$ 3 $$action_left$$ 4";
    xr_string res;
    int k = 0;
    const char* b;
#define ACTION_STR "$$ACTION_"

    //.	int LEN				= (int)xr_strlen(ACTION_STR);
#define LEN 9

    string256 buff;
    string256 srcbuff;
    bool b_hit = false;

    while ((b = strstr(str + k, ACTION_STR)) != 0)
    {
        buff[0] = 0;
        srcbuff[0] = 0;
        res.append(str + k, b - str - k);
        const char* e = strstr(b + LEN, "$$");

        int len = (int)(e - b - LEN);

        strncpy_s(srcbuff, b + LEN, len);
        srcbuff[len] = 0;
        GetActionAllBinding(srcbuff, buff, sizeof(buff));
        res.append(buff, xr_strlen(buff));

        k = (int)(b - str);
        k += len;
        k += LEN;
        k += 2;
        b_hit = true;
    };

    if (k < (int)xr_strlen(str))
    {
        res.append(str + k);
    }

    if (b_hit && bFirst)
        pData->m_string_key_binding[skey] = str;

    return STRING_VALUE(res.c_str());
}

STRING_VALUE CStringTable::translate(const STRING_ID& str_id) const
{
    VERIFY(pData);

    if (pData->m_StringTable.find(str_id) != pData->m_StringTable.end())
        return pData->m_StringTable[str_id];
    else
        return str_id;
}
