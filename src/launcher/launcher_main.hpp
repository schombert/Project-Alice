#pragma once

#define ALICE_NO_ENTRY_POINT 1

#include "common_types.cpp"
#include "lua_alice_api.cpp"
#include "system_state.cpp"
#include "dcon_oos_reporter_generated.cpp"
#ifndef INCREMENTAL
#include "parsers.cpp"
#include "text.cpp"
#include "float_from_chars.cpp"
#include "fonts.cpp"
#include "texture.cpp"
#include "date_interface.cpp"
#include "serialization.cpp"
#include "nations.cpp"
#include "culture.cpp"
#include "military.cpp"
#include "debug_string_convertions.cpp"
#include "modifiers.cpp"
#include "province.cpp"
#include "triggers.cpp"
#include "effects.cpp"
#include "economy_stats.cpp"
#include "economy.cpp"
#include "economy_pops.cpp"
#include "economy_trade_routes.cpp"
#include "economy_government.cpp"
#include "economy_production.cpp"
#include "national_budget.cpp"
#include "construction.cpp"
#include "advanced_province_buildings.cpp"
#include "demographics.cpp"
#include "bmfont.cpp"
#include "rebels.cpp"
#include "politics.cpp"
#include "events.cpp"
#include "labour_details.cpp"
#include "gui_graphics.cpp"
#include "gui_common_elements.cpp"
#include "widgets/table.cpp"
#include "gui_trigger_tooltips.cpp"
#include "gui_effect_tooltips.cpp"
#include "gui_modifier_tooltips.cpp"
#include "gui_leader_tooltip.cpp"
#include "gui_leader_select.cpp"
#include "gui_production_window.cpp"
#include "gui_province_window.cpp"
#include "gui_population_window.cpp"
#include "gui_context_window.cpp"
#include "gui_factory_refit_window.cpp"
#include "province_tiles.cpp"
#include "immediate_mode.cpp"
#include "economy_viewer.cpp"
#include "gui_technology_window.cpp"
#include "gui_error_window.cpp"
#include "game_scene.cpp"
#include "SHA512.cpp"
#include "commands.cpp"
#include "network.cpp"
#include "diplomatic_messages.cpp"
#include "notifications.cpp"
#include "map_tooltip.cpp"
#include "unit_tooltip.cpp"
#include "ai.cpp"
#include "ai_campaign.cpp"
#include "ai_campaign_values.cpp"
#include "ai_focuses.cpp"
#include "ai_alliances.cpp"
#include "ai_influence.cpp"
#include "ai_economy.cpp"
#include "ai_war.cpp"
#include "fif_triggers.cpp"
#include "map_modes.cpp"
#include "platform_specific.cpp"
#include "opengl_wrapper.cpp"
#include "prng.cpp"
#include "blake2.cpp"
#include "zstd.cpp"
#include "pcp.cpp"
#endif
#include "gui_element_types.cpp"
#include "gui_main_menu.cpp"
#include "gui_console.cpp"
#include "gui_event.cpp"
#include "gui_message_settings_window.cpp"

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "fonts.hpp"
#include "texture.hpp"
#include "text.hpp"
#include "prng.hpp"
#include "system_state.hpp"
#include "serialization.hpp"
#include "network.hpp"
#include "simple_fs.hpp"

namespace launcher {

static float scaling_factor = 1.0f;
static float dpi = 96.0f;
constexpr inline float base_width = 880.0f;
constexpr inline float base_height = 540.0f;

constexpr inline float caption_width = 837.0f;
constexpr inline float caption_height = 44.0f;

static int32_t mouse_x = 0;
static int32_t mouse_y = 0;

static std::string ip_addr = "127.0.0.1";
static std::string lobby_password = "";
static sys::player_name player_name;
static sys::player_name player_password;

static std::string requestedScenarioFileName;
static std::string enabledModsMask;
static bool autoBuild = false;
static bool headless = false;

enum class string_index : uint8_t {
	create_scenario,
	recreate_scenario,
	working,
	create_a_new_scenario,
	for_the_selected_mods,
	no_scenario_found,
	ip_address,
	lobby_password,
	nickname,
	singleplayer,
	multiplayer,
	start_game,
	host,
	join,
	mod_list,
	player_password,
	count,
};

//english
static std::string_view en_localised_strings[uint8_t(string_index::count)] = {
	"Create scenario",
	"Recreate scenario",
	"Working...",
	"Create a new scenario",
	"for the selected mods",
	"No scenario found",
	"IP Address",
	"Lobby Password",
	"Nickname",
	"Singleplayer",
	"Multiplayer",
	"Start game",
	"Host",
	"Join",
	"Mod list",
	"Player Password"
};
//turkish
static std::string_view tr_localised_strings[uint8_t(string_index::count)] = {
	"Senaryo oluştur",
	"Senaryoyu yeniden oluştur",
	"Çalışma...",
	"Seçilen modlar için yeni",
	"bir senaryo oluşturun",
	"Senaryo bulunamadı",
	"IP adresi",
	"Şifre",
	"Takma ad",
	"Tek oyunculu",
	"Çok Oyunculu",
	"Oyunu başlatmak",
	"Ev sahibi",
	"Katılmak",
	"Mod listesi",
	"Player Password"
};
//albanian
static std::string_view sq_localised_strings[uint8_t(string_index::count)] = {
	"Krijo skenar",
	"Rikrijo skenar",
	"Punon...",
	"Krijo një skenar të ri",
	"për modalitetet e zgjedhura",
	"Nuk u gjet asnjë skenar",
	"Adresa IP",
	"Fjalëkalimi",
	"Pseudonimi",
	"Lojtar i vetëm",
	"Shumë lojtarë",
	"Fillo lojen",
	"Mikpritës",
	"Bashkohu",
	"Lista e modës",
	"Player Password"

};
//spanish
static std::string_view es_localised_strings[uint8_t(string_index::count)] = {
	"Crear escenario",
	"Recrear escenario",
	"Trabajando...",
	"Crea un nuevo escenario",
	"para los mods seleccionados",
	"No se encontro el escenario",
	"Dirección IP",
	"Contraseña",
	"Alias",
	"Un jugador",
	"Multijugador",
	"Empezar juego",
	"Hostear",
	"Unirse",
	"Lista de mods",
	"Player Password"

};
//italian
static std::string_view it_localised_strings[uint8_t(string_index::count)] = {
	"Crea esenario",
	"Ricreare esenario",
	"Lavorando...",
	"Crea un nuovo esenario",
	"per i mod selezionati",
	"Scenario non trovato",
	"Indirizzo IP",
	"Password",
	"Alias",
	"Singolo",
	"Multigiocatore",
	"Inizia il gioco",
	"Ospite",
	"Partecipare",
	"Elenco delle mods",
	"Player Password"

};
//french
static std::string_view fr_localised_strings[uint8_t(string_index::count)] = {
	"Creer un scènario",
	"Recrèer le scènario",
	"Fonctionnement...",
	"Creer un nouveau scènario",
	"pour les mods sèlectionnès",
	"Scènario introuvable",
	"Addresse IP",
	"Passe",
	"Alias",
	"Solo",
	"Multijoueur",
	"Dèmarrer jeu",
	"Hõte",
	"Rejoindre",
	"Liste des modifications",
	"Player Password"

};
//portuguese
static std::string_view po_localised_strings[uint8_t(string_index::count)] = {
	"Criar cenário",
	"Recriar cenário",
	"Trabalhando...",
	"Crie un novo cenário para",
	"os mods selecionados",
	"Cenário não encontrado",
	"Endereço IP",
	"Senha",
	"Alias",
	"Unjogador",
	"Multijogador",
	"Começar o jogo",
	"Hospedar",
	"Junte-se",
	"Lista de modificaçães",
	"Player Password"

};
//deutsch
static std::string_view de_localised_strings[uint8_t(string_index::count)] = {
	"Szenario erstellen",
	"Szenario neu erstellen",
	"Arbeitet...",
	"Neues Szenario für die",
	"ausgewählten mods erstellen",
	"Szenario nicht gefunden",
	"IP-Adresse",
	"Passwort",
	"Alias",
	"Einzelspieler",
	"Mehrspieler",
	"Spiel starten",
	"Hosten",
	"Teilnehmen",
	"Liste der Modifikationen",
	"Player Password"

};
//swedish
static std::string_view sv_localised_strings[uint8_t(string_index::count)] = {
	"Skapa scenario",
	"Återskapa scenario",
	"Arbetssått...",
	"Skepa ett nytt scenario",
	"för de valda mods",
	"Scenario hittades inte",
	"IP-adress",
	"Lösenord",
	"Alias",
	"Einselspalet",
	"Merspalet",
	"Starta spelet",
	"Gå med",
	"Vara värd",
	"Lista åver åndriggar",
	"Player Password"

};
//chinese
static std::string_view zh_localised_strings[uint8_t(string_index::count)] = {
	"创建方案",
	"重新创建方案",
	"工作中……",
	"为所选模组",
	"创建新方案",
	"未找到方案",
	"IP 地址",
	"主机密码",
	"昵称",
	"单人游戏",
	"多人游戏",
	"开始游戏",
	"主持",
	"加入",
	"模组列表",
	"玩家密码"

};
//arabic
static std::string_view ar_localised_strings[uint8_t(string_index::count)] = {
	"إنشاء السيناريو",
	"إعادة إنشاء السيناريو",
	"عمل...",
	"إنشاء سيناريو جديد",
	"للوضع المحدد",
	"لم يتم العثور على المشهد",
	"عنوان IP",
	"كلمة المرور",
	"كنية",
	"لاعب واحد",
	"متعددة اللاعبين",
	"بدء اللعبة",
	"يستضيف",
	"ينضم",
	"قائمة وزارة الدفاع",
	"Player Password"

};
//norwegian
static std::string_view no_localised_strings[uint8_t(string_index::count)] = {
	"Lag scenario",
	"Gjenskape scenario",
	"Arbeider...",
	"Lag et nytt scenario",
	"for de valgte modsene",
	"Ingen scenarioer funnet",
	"IP adresse",
	"Passord",
	"Kallenavn",
	"Enkeltspiller",
	"Flerspiller",
	"Start spill",
	"Vert",
	"Bli med",
	"Mod liste",
	"Player Password"

};
//romanian
static std::string_view ro_localised_strings[uint8_t(string_index::count)] = {
	"Creați script",
	"Scenariu de recenzie",
	"Lucru...",
	"Creați un nou script",
	"pentru moduri selectate",
	"Nu a fost găsit niciun script",
	"adresa IP",
	"Parola",
	"Poreclă",
	"Un singur jucator",
	"Jucători multipli",
	"Începeți jocul",
	"Gazdă",
	"A te alatura",
	"Lista de moduri",
	"Player Password"

};
//russian
static std::string_view ru_localised_strings[uint8_t(string_index::count)] = {
	"Создать сценарий",
	"Воссоздать сценарий",
	"В работе...",
	"Создать новый сценарий",
	"Для выбранного мода",
	"сценарий не найден",
	"IP адрес",
	"Пароль лобби",
	"Никнейм",
	"Одиночная игра",
	"Мультиплеер",
	"Начать игру",
	"Сервер",
	"Присоединиться",
	"Список модов",
	"Пароль игрока"
};
//polish
static std::string_view pl_localised_strings[uint8_t(string_index::count)] = {
	"Stwórz scenariusz",
	"Odtwórz scenariusz",
	"Pracujący...",
	"Utwórz nowy scenariusz",
	"dla wybranych modów",
	"Nie znaleziono scenariusza",
	"Adres IP",
	"Hasło",
	"Przezwisko",
	"Jeden gracz",
	"Tryb wieloosobowy",
	"Rozpocząć grę",
	"Gospodarz",
	"Dołączyć",
	"Lista modów",
	"Player Password"

};
//bulgarian
static std::string_view bg_localised_strings[uint8_t(string_index::count)] = {
	"Създайте сценарий",
	"Пресъздайте сценарий",
	"Работи...",
	"Създайте нов сценарий",
	"за избраните модове",
	"Няма намерен сценарий",
	"IP адрес",
	"Парола",
	"Псевдоним",
	"Един играч",
	"Мултиплейър",
	"Започни игра",
	"Домакин",
	"Присъединяване",
	"Мод списък",
	"Парола на играча"

};
//catalan
static std::string_view ca_localised_strings[uint8_t(string_index::count)] = {
	"Crea un escenari",
	"Recrea l'escenari",
	"Treball...",
	"Creeu un nou escenari",
	"per als mods seleccionats",
	"No s'ha trobat cap escenari",
	"Adreça IP",
	"Contrasenya",
	"Pseudònim",
	"Sol jugador",
	"Multijugador",
	"Començar el joc",
	"Amfitrió",
	"Uneix - te",
	"Llista de modificacions",
	"Player password"
};
//czech
static std::string_view cs_localised_strings[uint8_t(string_index::count)] = {
	"Vytvořte scénář",
	"Znovu vytvořit scénář",
	"Pracovní...",
	"Vytvořte nový scénář",
	"pro vybrané mody",
	"Nebyl nalezen žádný scénář",
	"IP adresa",
	"Heslo",
	"Přezdívka",
	"Pro jednoho hráče",
	"Pro více hráčů",
	"Začít hru",
	"Hostitel",
	"Připojit",
	"Seznam modů",
	"Player password"

};
//danish
static std::string_view da_localised_strings[uint8_t(string_index::count)] = {
	"Opret scenarie",
	"Genskab scenariet",
	"Arbejder...",
	"Opret et nyt scenarie",
	"for de valgte mods",
	"Intet scenarie fundet",
	"IP - adresse",
	"Adgangskode",
	"Kaldenavn",
	"En spiller",
	"Flere spillere",
	"Start Spil",
	"Vært",
	"Tilslutte",
	"Mod liste",
	"Player password"

};
//greek
static std::string_view el_localised_strings[uint8_t(string_index::count)] = {
	"Δημιουργία σεναρίου",
	"Αναδημιουργήστε το σενάριο",
	"Εργαζόμενος...",
	"Δημιουργήστε ένα νέο σενάριο",
	"για τα επιλεγμένα mods",
	"Δεν βρέθηκε κανένα σενάριο",
	"Διεύθυνση IP",
	"Κωδικός πρόσβασης",
	"Παρατσούκλι",
	"Μονος παιχτης",
	"Λειτουργία για πολλούς παίκτες",
	"Ξεκίνα το παιχνίδι",
	"Πλήθος",
	"Συμμετοχή",
	"Λίστα mod",
	"Player password"

};
//finnish
static std::string_view fi_localised_strings[uint8_t(string_index::count)] = {
	"Luo skenaario",
	"Luo skenaario uudelleen",
	"Työskentelee...",
	"Luo uusi skenaario",
	"valituille modeille",
	"Skenaariota ei löytynyt",
	"IP - osoite",
	"Salasana",
	"Nimimerkki",
	"Yksinpeli",
	"Moninpeli",
	"Aloita peli",
	"Isäntä",
	"Liittyä seuraan",
	"Mod lista",
	"Player password"

};
//hebrew
static std::string_view he_localised_strings[uint8_t(string_index::count)] = {
	"צור תרחיש",
	"ליצור מחדש תרחיש",
	"עובד...",
	"צור תרחיש חדש עבור",
	"המצבים שנבחרו",
	"לא נמצא תרחיש",
	"כתובת פרוטוקול אינטרנט",
	"סיסמה",
	"כינוי",
	"שחקן יחיד",
	"רב משתתפים",
	"התחל משחק",
	"מנחה",
	"לְהִצְטַרֵף",
	"רשימת השינויים במשחק",
	"Player password"

};
//hungarian
static std::string_view hu_localised_strings[uint8_t(string_index::count)] = {
	"Forgatókönyv létrehozása",
	"Forgatókönyv újbóli létrehozása",
	"Dolgozó...",
	"Hozzon létre egy új forgatókönyvet",
	"a kiválasztott modokhoz",
	"Nem található forgatókönyv",
	"IP - cím",
	"Jelszó",
	"Becenév",
	"Egyjátékos",
	"Többjátékos",
	"Játék kezdése",
	"Házigazda",
	"Csatlakozik",
	"Mod lista",
	"Player password"

};
//dutch
static std::string_view nl_localised_strings[uint8_t(string_index::count)] = {
	"Scenario maken",
	"Scenario opnieuw maken",
	"Werken...",
	"Maak een nieuw scenario",
	"voor de geselecteerde mods",
	"Geen scenario gevonden",
	"IP adres",
	"Wachtwoord",
	"Bijnaam",
	"Een speler",
	"Meerdere spelers",
	"Start het spel",
	"Gastheer",
	"Meedoen",
	"Mod - lijst",
	"Player password"

};
//lithuanian
static std::string_view lt_localised_strings[uint8_t(string_index::count)] = {
	"Sukurti scenarijų",
	"Atkurti scenarijų",
	"Dirba...",
	"Sukurkite naują pasirinktų",
	"modifikacijų scenarijų",
	"Scenarijus nerastas",
	"IP adresas",
	"Slaptažodis",
	"Slapyvardis",
	"Vieno žaidėjo",
	"Kelių žaidėjų",
	"Pradėti žaidimą",
	"Šeimininkas",
	"Prisijunk",
	"Modifikacijų sąrašas",
	"Player password"

};
//latvian
static std::string_view lv_localised_strings[uint8_t(string_index::count)] = {
	"Izveidojiet scenāriju",
	"Atkārtoti izveidojiet scenāriju",
	"Strādā...",
	"Izveidojiet jaunu scenāriju",
	"atlasītajiem modiem",
	"Nav atrasts neviens scenārijs",
	"IP adrese",
	"Parole",
	"Segvārds",
	"Viens spēlētājs",
	"Vairāku spēlētāju spēle",
	"Sākt spēli",
	"Uzņēmēja",
	"Pievienojieties",
	"Modu saraksts",
	"Player password"

};
//estonian
static std::string_view et_localised_strings[uint8_t(string_index::count)] = {
	"Loo stsenaarium",
	"Loo stsenaarium uuesti",
	"Töötab...",
	"Looge valitud modifikatsioonide",
	"jaoks uus stsenaarium",
	"Stsenaariumi ei leitud",
	"IP - aadress",
	"Parool",
	"Hüüdnimi",
	"Üksik mängija",
	"Mitmikmäng",
	"Alusta mängu",
	"Host",
	"Liitu",
	"Modifikatsioonide loend",
	"Player password"

};
//hindi
static std::string_view hi_localised_strings[uint8_t(string_index::count)] = {
	"परिदृश्य बनाएँ",
	"परिदृश्य फिर से बनाएँ",
	"कार्य कर रहा है...",
	"चयनित मॉड के लिए",
	"एक नया परिदृश्य बनाएँ",
	"कोई परिदृश्य नहीं मिला",
	"आईपी पता",
	"पासवर्ड",
	"उपनाम",
	"एकल खिलाड़ी",
	"मल्टीप्लेयर",
	"खेल शुरू करें",
	"होस्ट",
	"जॉइन करें",
	"मॉड सूची",
	"Player password"

};
//vietnamese
static std::string_view vi_localised_strings[uint8_t(string_index::count)] = {
	"Tạo kịch bản",
	"Kịch bản tái tạo",
	"Đang làm việc...",
	"Tạo một kịch bản mới cho",
	"các mod đã chọn",
	"Không tìm thấy kịch bản",
	"Địa chỉ IP",
	"Mật khẩu",
	"Tên nick",
	"Người chơi đơn",
	"Nhiều người chơi",
	"Bắt đầu trò chơi",
	"Chủ nhà",
	"Tham gia",
	"Danh sách mod",
	"Player password"

};
//armenian
static std::string_view hy_localised_strings[uint8_t(string_index::count)] = {
	"Ստեղծեք սցենար",
	"Վերստեղծեք սցենարը",
	"Աշխատանքային ...",
	"Ստեղծեք նոր սցենար",
	"ընտրված ռեժիմների համար",
	"Ոչ մի սցենար չի գտնվել",
	"IP հասցե",
	"Գաղտնաբառ",
	"Մականուն",
	"Միայնակ խաղացող",
	"Բազմապատկիչ",
	"Սկսել խաղը",
	"Հյուրընկալող",
	"Միանալ",
	"Mod ուցակ",
	"Player password"

};
//ukrainian
static std::string_view uk_localised_strings[uint8_t(string_index::count)] = {
	"Створити сценарій",
	"Сценарій огляду",
	"Працює ...",
	"Створіть новий сценарій",
	"для вибраних мод",
	"Не знайдено сценарію",
	"IP - адреса",
	"Пароль",
	"Прізвище",
	"Один гравець",
	"Мультиплеєр",
	"Почніть гру",
	"Господар",
	"З'єднувати",
	"Список мод",
	"Player password"

};
static std::string_view* localised_strings = &en_localised_strings[0];

struct ui_active_rect {
	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;
};

constexpr inline int32_t ui_obj_close = 0;
constexpr inline int32_t ui_obj_list_left = 1;
constexpr inline int32_t ui_obj_list_right = 2;
constexpr inline int32_t ui_obj_create_scenario = 3;
constexpr inline int32_t ui_obj_play_game = 4;
constexpr inline int32_t ui_obj_host_game = 5;
constexpr inline int32_t ui_obj_join_game = 6;
constexpr inline int32_t ui_obj_ip_addr = 7;
constexpr inline int32_t ui_obj_password = 8;
constexpr inline int32_t ui_obj_player_name = 9;
constexpr inline int32_t ui_obj_player_password = 10;


constexpr inline int32_t ui_list_count = 14;

constexpr inline int32_t ui_list_first = 11;
constexpr inline int32_t ui_list_checkbox = 0;
constexpr inline int32_t ui_list_move_up = 1;
constexpr inline int32_t ui_list_move_down = 2;
constexpr inline int32_t ui_list_end = ui_list_first + ui_list_count * 3;

constexpr inline int32_t ui_row_height = 32;

constexpr inline float list_text_right_align = 420.0f;

static int32_t obj_under_mouse = -1;

static bool game_dir_not_found = false;

constexpr inline ui_active_rect ui_rects[] = {
	ui_active_rect{ 880 - 31,  0 , 31, 31}, // close
	ui_active_rect{ 30, 208, 21, 93}, // left
	ui_active_rect{ 515, 208, 21, 93}, // right
	ui_active_rect{ 555, 48, 286, 33 }, // create scenario
	ui_active_rect{ 555, 48 + 156 * 1, 286, 33 }, // play game
	ui_active_rect{ 555, 48 + 156 * 2 + 36 * 0, 138, 33 }, // host game
	ui_active_rect{ 703, 48 + 156 * 2 + 36 * 0, 138, 33 }, // join game
	ui_active_rect{ 555, 54 + 156 * 2 + 36 * 2, 138, 23 }, // ip address textbox
	ui_active_rect{ 555, 54 + 156 * 2 + 36 * 3 + 12, 138, 23 }, // lobby_password textbox
	ui_active_rect{ 703, 54 + 156 * 2 + 36 * 2, 138, 23 }, // player name textbox
	ui_active_rect{ 703, 54 + 156 * 2 + 36 * 3 + 12, 138, 23 }, // player_password textbox

	ui_active_rect{ 60 + 6, 75 + 32 * 0 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 0 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 0 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 1 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 1 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 1 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 2 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 2 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 2 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 3 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 3 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 3 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 4 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 4 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 4 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 5 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 5 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 5 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 6 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 6 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 6 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 7 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 7 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 7 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 8 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 8 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 8 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 9 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 9 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 9 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 10 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 10 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 10 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 11 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 11 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 11 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 12 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 12 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 12 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 13 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 13 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 13 + 4, 24, 24 },
};

static std::vector<parsers::mod_file> mod_list;

struct scenario_file {
	native_string file_name;
	sys::mod_identifier ident;
};

static std::vector<scenario_file> scenario_files;
static native_string selected_scenario_file;
static uint32_t max_scenario_count = 0;
static std::atomic<bool> file_is_ready = true;

static int32_t frame_in_list = 0;

bool update_under_mouse() { // return if the selected object (if any) has changed
	for(int32_t i = 0; i < ui_list_end; ++i) {
		if(mouse_x >= ui_rects[i].x && mouse_x < ui_rects[i].x + ui_rects[i].width
			&& mouse_y >= ui_rects[i].y && mouse_y < ui_rects[i].y + ui_rects[i].height) {

			if(obj_under_mouse != i) {
				obj_under_mouse = i;
				return true;
			} else {
				return false;
			}
		}
	}

	if(obj_under_mouse != -1) {
		obj_under_mouse = -1;
		return true;
	} else {
		return false;
	}
}

void recursively_remove_from_list(parsers::mod_file& mod) {
	for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
		if(mod_list[i].mod_selected) {
			if(std::find(mod_list[i].dependent_mods.begin(), mod_list[i].dependent_mods.end(), mod.name_) != mod_list[i].dependent_mods.end()) {
				mod_list[i].mod_selected = false;
				recursively_remove_from_list(mod_list[i]);
			}
		}
	}
}
void recursively_add_to_list(parsers::mod_file& mod) {
	for(auto& dep : mod.dependent_mods) {
		for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
			if(!mod_list[i].mod_selected && mod_list[i].name_ == dep) {
				mod_list[i].mod_selected = true;
				recursively_add_to_list(mod_list[i]);
			}
		}
	}
}

bool transitively_depends_on_internal(parsers::mod_file const& moda, parsers::mod_file const& modb, std::vector<bool>& seen_indices) {
	for(auto& dep : moda.dependent_mods) {
		if(dep == modb.name_)
			return true;

		for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
			if(seen_indices[i] == false && mod_list[i].name_ == dep) {
				seen_indices[i] = true;
				if(transitively_depends_on_internal(mod_list[i], modb, seen_indices))
					return true;
			}
		}
	}
	return false;
}

bool transitively_depends_on(parsers::mod_file const& moda, parsers::mod_file const& modb) {
	std::vector<bool> seen_indices;
	seen_indices.resize(mod_list.size());

	return transitively_depends_on_internal(moda, modb, seen_indices);
}

void enforce_list_order() {
	std::stable_sort(mod_list.begin(), mod_list.end(), [&](parsers::mod_file const& a, parsers::mod_file const& b) {
		if(a.mod_selected && b.mod_selected) {
			return transitively_depends_on(b, a);
		} else if(a.mod_selected && !b.mod_selected) {
			return true;
		} else if(!a.mod_selected && b.mod_selected) {
			return false;
		} else {
			return a.name_ < b.name_;
		}
	});
}

bool nth_item_can_move_up(int32_t n) {
	if(n == 0)
		return false;
	if(transitively_depends_on(mod_list[n], mod_list[n - 1]))
		return false;

	return true;
}
bool nth_item_can_move_down(int32_t n) {
	if(n >= int32_t(mod_list.size()) - 1)
		return false;
	if(mod_list[n + 1].mod_selected == false)
		return false;
	if(transitively_depends_on(mod_list[n + 1], mod_list[n]))
		return false;

	return true;
}

native_string produce_mod_path() {
	simple_fs::file_system dummy;
	simple_fs::add_root(dummy, NATIVE("."));

	for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
		if(mod_list[i].mod_selected == false)
			break;

		mod_list[i].add_to_file_system(dummy);
	}

	return simple_fs::extract_state(dummy);
}

void save_playername() {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	simple_fs::write_file(settings_location, NATIVE("player_name.dat"), (const char*)&player_name, sizeof(player_name));
}

void save_playerpassw() {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	simple_fs::write_file(settings_location, NATIVE("player_passw.dat"), (const char*)&player_password, sizeof(player_password));
}

void load_playername() {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	if (auto player_name_file = simple_fs::open_file(settings_location, NATIVE("player_name.dat")); player_name_file) {
		auto contents = simple_fs::view_contents(*player_name_file);
		const sys::player_name* p = (const sys::player_name*)contents.data;
		if (contents.file_size >= sizeof(*p)) {
			launcher::player_name = launcher::player_name.from_string_view(std::string(std::begin(p->data), std::end(p->data)));
		}
	}
	else {
		srand(time(NULL));
		launcher::player_name = launcher::player_name.from_string_view(std::to_string(int32_t(rand())));
	}
}

void load_playerpassw() {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	if (auto player_name_file = simple_fs::open_file(settings_location, NATIVE("player_passw.dat")); player_name_file) {
		auto contents = simple_fs::view_contents(*player_name_file);
		const sys::player_name* p = (const sys::player_name*)contents.data;
		if (contents.file_size >= sizeof(*p)) {
			launcher::player_password = launcher::player_password.from_string_view(std::string(std::begin(p->data), std::end(p->data)));
		}
	}
	else {
		srand(time(NULL));
		launcher::player_password = launcher::player_password.from_string_view(std::to_string(int32_t(rand())));
	}
}

native_string to_hex(uint64_t v) {
	native_string ret;
	constexpr native_char digits[] = NATIVE("0123456789ABCDEF");
	do {
		ret += digits[v & 0x0F];
		v = v >> 4;
	} while(v != 0);

	return ret;
}

static GLfloat global_square_data[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f };
static GLfloat global_square_right_data[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
static GLfloat global_square_left_data[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat global_square_flipped_data[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat global_square_right_flipped_data[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
static GLfloat global_square_left_flipped_data[] = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

static GLuint ui_shader_program = 0;

static GLuint global_square_vao = 0;
static GLuint global_square_buffer = 0;
static GLuint global_square_right_buffer = 0;
static GLuint global_square_left_buffer = 0;
static GLuint global_square_flipped_buffer = 0;
static GLuint global_square_right_flipped_buffer = 0;
static GLuint global_square_left_flipped_buffer = 0;

static GLuint sub_square_buffers[64] = { 0 };

void load_global_squares() {
	glGenBuffers(1, &global_square_buffer);

	// Populate the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, global_square_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &global_square_vao);
	glBindVertexArray(global_square_vao);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // texture coordinates

	glBindVertexBuffer(0, global_square_buffer, 0, sizeof(GLfloat) * 4);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);					 // position
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2);	// texture coordinates
	glVertexAttribBinding(0, 0);											// position -> to array zero
	glVertexAttribBinding(1, 0);											 // texture coordinates -> to array zero

	glGenBuffers(1, &global_square_left_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_left_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_left_data, GL_STATIC_DRAW);

	glGenBuffers(1, &global_square_right_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_right_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_right_data, GL_STATIC_DRAW);

	glGenBuffers(1, &global_square_right_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_right_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_right_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &global_square_left_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_left_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_left_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &global_square_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(64, sub_square_buffers);
	for(uint32_t i = 0; i < 64; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, sub_square_buffers[i]);

		float const cell_x = static_cast<float>(i & 7) / 8.0f;
		float const cell_y = static_cast<float>((i >> 3) & 7) / 8.0f;

		GLfloat global_sub_square_data[] = { 0.0f, 0.0f, cell_x, cell_y, 0.0f, 1.0f, cell_x, cell_y + 1.0f / 8.0f, 1.0f, 1.0f,
				cell_x + 1.0f / 8.0f, cell_y + 1.0f / 8.0f, 1.0f, 0.0f, cell_x + 1.0f / 8.0f, cell_y };

		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_sub_square_data, GL_STATIC_DRAW);
	}
}


namespace ogl {
namespace parameters {

inline constexpr GLuint screen_width = 0;
inline constexpr GLuint screen_height = 1;
inline constexpr GLuint drawing_rectangle = 2;

inline constexpr GLuint border_size = 6;
inline constexpr GLuint inner_color = 7;
inline constexpr GLuint subrect = 10;

inline constexpr GLuint enabled = 4;
inline constexpr GLuint disabled = 3;
inline constexpr GLuint border_filter = 0;
inline constexpr GLuint filter = 1;
inline constexpr GLuint no_filter = 2;
inline constexpr GLuint sub_sprite = 5;
inline constexpr GLuint use_mask = 6;
inline constexpr GLuint progress_bar = 7;
inline constexpr GLuint frame_stretch = 8;
inline constexpr GLuint piechart = 9;
inline constexpr GLuint barchart = 10;
inline constexpr GLuint linegraph = 11;
inline constexpr GLuint tint = 12;
inline constexpr GLuint interactable = 13;
inline constexpr GLuint interactable_disabled = 14;
inline constexpr GLuint subsprite_b = 15;
inline constexpr GLuint atlas_index = 18;

} // namespace parameters

enum class color_modification {
	none, disabled, interactable, interactable_disabled
};

struct color3f {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

inline auto map_color_modification_to_index(color_modification e) {
	switch(e) {
	case color_modification::disabled:
		return parameters::disabled;
	case color_modification::interactable:
		return parameters::interactable;
	case color_modification::interactable_disabled:
		return parameters::interactable_disabled;
	default:
	case color_modification::none:
		return parameters::enabled;
	}
}

void bind_vertices_by_rotation(ui::rotation r, bool flipped) {
	switch(r) {
	case ui::rotation::upright:
		if(!flipped)
			glBindVertexBuffer(0, global_square_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, global_square_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	case ui::rotation::r90_left:
		if(!flipped)
			glBindVertexBuffer(0, global_square_left_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, global_square_left_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	case ui::rotation::r90_right:
		if(!flipped)
			glBindVertexBuffer(0, global_square_right_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, global_square_right_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	}
}

void render_textured_rect(color_modification enabled, int32_t ix, int32_t iy, int32_t iwidth, int32_t iheight, GLuint texture_handle, ui::rotation r, bool flipped) {
	float x = float(ix);
	float y = float(iy);
	float width = float(iwidth);
	float height = float(iheight);

	glBindVertexArray(global_square_vao);

	bind_vertices_by_rotation(r, flipped);

	glUniform4f(glGetUniformLocation(ui_shader_program, "d_rect"), x, y, width, height);
	// glUniform4f(glGetUniformLocation(ui_shader_program, "d_rect"), 0, 0, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::no_filter };
	glUniform2ui(glGetUniformLocation(ui_shader_program, "subroutines_index"), subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void internal_text_render(std::string_view str, float baseline_x, float baseline_y, float size, ::text::font& f) {
	hb_buffer_clear_contents(f.hb_buf);
	hb_buffer_add_utf8(f.hb_buf, str.data(), int(str.size()), 0, int(str.size()));
	hb_buffer_guess_segment_properties(f.hb_buf);
	hb_shape(f.hb_font_face, f.hb_buf, NULL, 0);
	unsigned int glyph_count = 0;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(f.hb_buf, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(f.hb_buf, &glyph_count);
	float x = baseline_x;
	for(unsigned int i = 0; i < glyph_count; i++) {
		f.make_glyph(glyph_info[i].codepoint);
	}
	for(unsigned int i = 0; i < glyph_count; i++) {
		hb_codepoint_t glyphid = glyph_info[i].codepoint;
		auto gso = f.glyph_positions[glyphid];
		float x_advance = float(glyph_pos[i].x_advance) / (float((1 << 6) * text::magnification_factor));
		float x_offset = float(glyph_pos[i].x_offset) / (float((1 << 6) * text::magnification_factor)) + float(gso.x);
		float y_offset = float(gso.y) - float(glyph_pos[i].y_offset) / (float((1 << 6) * text::magnification_factor));
		if(glyphid != FT_Get_Char_Index(f.font_face, ' ')) {
			glBindVertexBuffer(0, sub_square_buffers[f.glyph_positions[glyphid].texture_slot & 63], 0, sizeof(GLfloat) * 4);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, f.textures[f.glyph_positions[glyphid].texture_slot >> 6]);
			glUniform4f(glGetUniformLocation(ui_shader_program, "d_rect"), x + x_offset * size / 64.f, baseline_y + y_offset * size / 64.f, size, size);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		x += x_advance * size / 64.f;
	}
}

void render_new_text(std::string_view sv, color_modification enabled, float x, float y, float size, color3f const& c, ::text::font& f) {
	glUniform3f(glGetUniformLocation(ui_shader_program, "inner_color"), c.r, c.g, c.b);
	glUniform1f(glGetUniformLocation(ui_shader_program, "border_size"), 0.08f * 16.0f / size);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::filter };
	glUniform2ui(glGetUniformLocation(ui_shader_program, "subroutines_index"), subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call
	internal_text_render(sv, x, y + size, size, f);
}

} // launcher::ogl

static ::text::font_manager font_collection; //keep static because it uninits FT lib on destructor
static ::text::font fonts[2];

static ::ogl::texture bg_tex;
static ::ogl::texture left_tex;
static ::ogl::texture right_tex;
static ::ogl::texture close_tex;
static ::ogl::texture big_button_tex;
static ::ogl::texture empty_check_tex;
static ::ogl::texture check_tex;
static ::ogl::texture up_tex;
static ::ogl::texture down_tex;
static ::ogl::texture line_bg_tex;
static ::ogl::texture big_l_button_tex;
static ::ogl::texture big_r_button_tex;
static ::ogl::texture warning_tex;

float base_text_extent(char const* codepoints, uint32_t count, int32_t size, text::font& f) {
	hb_buffer_clear_contents(f.hb_buf);
	hb_buffer_add_utf8(f.hb_buf, codepoints, int(count), 0, int(count));
	hb_buffer_guess_segment_properties(f.hb_buf);
	hb_shape(f.hb_font_face, f.hb_buf, NULL, 0);
	unsigned int glyph_count = 0;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(f.hb_buf, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(f.hb_buf, &glyph_count);
	float x = 0.0f;
	for(unsigned int i = 0; i < glyph_count; i++) {
		f.make_glyph(glyph_info[i].codepoint);
	}
	for(unsigned int i = 0; i < glyph_count; i++) {
		hb_codepoint_t glyphid = glyph_info[i].codepoint;
		auto gso = f.glyph_positions[glyphid];
		float x_advance = float(glyph_pos[i].x_advance) / (float((1 << 6) * text::magnification_factor));
		float x_offset = float(glyph_pos[i].x_offset) / (float((1 << 6) * text::magnification_factor)) + float(gso.x);
		float y_offset = float(gso.y) - float(glyph_pos[i].y_offset) / (float((1 << 6) * text::magnification_factor));
		x += x_advance * size / 64.f;
	}
	return x;
}

static int32_t active_textbox = -1;
bool is_cursor_visible = true;

}
