#define ALICE_NO_ENTRY_POINT 1

#include "common_types.cpp"
#include "system_state.cpp"
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

#ifndef UNICODE
#define UNICODE
#endif
#include <Windowsx.h>
#include <shellapi.h>
#include "Objbase.h"
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"
#include "wglew.h"
#include <cassert>
#include "resource.h"
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "icu.lib")
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
static boolean autoBuild = false;

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
	"密码",
	"昵称",
	"单人游戏",
	"多人游戏",
	"开始游戏",
	"主持",
	"加入",
	"模组列表",
	"Player Password"

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

static HWND m_hwnd = nullptr;

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

static HDC opengl_window_dc = nullptr;
static void* opengl_context = nullptr;

void create_opengl_context() {
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	HDC window_dc = opengl_window_dc;

	int const pixel_format = ChoosePixelFormat(window_dc, &pfd);
	SetPixelFormat(window_dc, pixel_format, &pfd);

	auto handle_to_ogl_dc = wglCreateContext(window_dc);
	wglMakeCurrent(window_dc, handle_to_ogl_dc);

	if(glewInit() != 0) {
		window::emit_error_message("GLEW failed to initialize", true);
	}

	if(!wglewIsSupported("WGL_ARB_create_context")) {
		window::emit_error_message("WGL_ARB_create_context not supported", true);
	}

	// Explicitly request for OpenGL 3.1
	static const int attribs_3_1[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};
	opengl_context = wglCreateContextAttribsARB(window_dc, nullptr, attribs_3_1);
	if(opengl_context == nullptr) {
		window::emit_error_message("Unable to create WGL context", true);
	}

	wglMakeCurrent(window_dc, HGLRC(opengl_context));
	wglDeleteContext(handle_to_ogl_dc);

	if(wglewIsSupported("WGL_EXT_swap_control_tear") == 1) {
		wglSwapIntervalEXT(-1);
	} else if(wglewIsSupported("WGL_EXT_swap_control") == 1) {
		wglSwapIntervalEXT(1);
	} else {
		window::emit_error_message("WGL_EXT_swap_control_tear and WGL_EXT_swap_control not supported", true);
	}
}

void shutdown_opengl() {
	wglMakeCurrent(opengl_window_dc, nullptr);
	wglDeleteContext(HGLRC(opengl_context));
	opengl_context = nullptr;
}

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

void make_mod_file() {
	file_is_ready.store(false, std::memory_order::memory_order_seq_cst);
	auto path = produce_mod_path();
	std::thread file_maker([path]() {
		simple_fs::file_system fs_root;
		simple_fs::restore_state(fs_root, path);
		parsers::error_handler err("");
		auto root = get_root(fs_root);
		auto common = open_directory(root, NATIVE("common"));
		parsers::bookmark_context bookmark_context;
		if(auto f = open_file(common, NATIVE("bookmarks.txt")); f) {
			auto bookmark_content = simple_fs::view_contents(*f);
			err.file_name = "bookmarks.txt";
			parsers::token_generator gen(bookmark_content.data, bookmark_content.data + bookmark_content.file_size);
			parsers::parse_bookmark_file(gen, err, bookmark_context);
			assert(!bookmark_context.bookmark_dates.empty());
		} else {
			err.accumulated_errors += "File common/bookmarks.txt could not be opened\n";
		}

		sys::checksum_key scenario_key;

		for(uint32_t date_index = 0; date_index < uint32_t(bookmark_context.bookmark_dates.size()); date_index++) {
			err.accumulated_errors.clear();
			err.accumulated_warnings.clear();
			//
			auto game_state = std::make_unique<sys::state>();
			simple_fs::restore_state(game_state->common_fs, path);
			game_state->load_scenario_data(err, bookmark_context.bookmark_dates[date_index].date_);
			if(err.fatal)
				break;
			if(date_index == 0) {
				auto sdir = simple_fs::get_or_create_scenario_directory();
				int32_t append = 0;
				auto time_stamp = uint64_t(std::time(0));
				auto base_name = to_hex(time_stamp);
				auto generated_scenario_name = base_name + NATIVE("-") + std::to_wstring(append) + NATIVE(".bin");
				while(simple_fs::peek_file(sdir, generated_scenario_name)) {
					++append;
				}
				// In this case we override the file
				if(requestedScenarioFileName != "") {
					generated_scenario_name = simple_fs::utf8_to_native(requestedScenarioFileName);
				}
				++max_scenario_count;
				selected_scenario_file = generated_scenario_name;
				sys::write_scenario_file(*game_state, selected_scenario_file, max_scenario_count);
				if(auto of = simple_fs::open_file(sdir, selected_scenario_file); of) {
					auto content = view_contents(*of);
					auto desc = sys::extract_mod_information(reinterpret_cast<uint8_t const*>(content.data), content.file_size);
					if(desc.count != 0) {
						scenario_files.push_back(scenario_file{ selected_scenario_file , desc });
					}
				}
				std::sort(scenario_files.begin(), scenario_files.end(), [](scenario_file const& a, scenario_file const& b) {
					return a.ident.count > b.ident.count;
				});
				scenario_key = game_state->scenario_checksum;
			} else {
#ifndef NDEBUG
				sys::write_scenario_file(*game_state, std::to_wstring(date_index) + NATIVE(".bin"), 0);
#endif
				game_state->scenario_checksum = scenario_key;
				sys::write_save_file(*game_state, sys::save_type::bookmark, bookmark_context.bookmark_dates[date_index].name_);
			}
		}

		if(!err.accumulated_errors.empty() || !err.accumulated_warnings.empty()) {
			auto assembled_file = std::string("You can still play the mod, but it might be unstable\r\nThe following problems were encountered while creating the scenario:\r\n\r\nErrors:\r\n") + err.accumulated_errors + "\r\n\r\nWarnings:\r\n" + err.accumulated_warnings;
			auto pdir = simple_fs::get_or_create_settings_directory();
			simple_fs::write_file(pdir, NATIVE("scenario_errors.txt"), assembled_file.data(), uint32_t(assembled_file.length()));

			if(!err.accumulated_errors.empty()) {
				auto fname = simple_fs::get_full_name(pdir) + NATIVE("\\scenario_errors.txt");
				ShellExecuteW(
					nullptr,
					L"open",
					fname.c_str(),
					nullptr,
					nullptr,
					SW_NORMAL
				);
			}
		}
		file_is_ready.store(true, std::memory_order::memory_order_release);

		if(autoBuild) {
			PostMessageW(m_hwnd, WM_CLOSE, 0, 0);
		}
		else {
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}
	});

	file_maker.detach();
}

void find_scenario_file() {
	if(!file_is_ready.load(std::memory_order::memory_order_acquire))
		return;

	file_is_ready.store(false, std::memory_order::memory_order_seq_cst);
	selected_scenario_file = NATIVE("");
	auto mod_path = produce_mod_path();

	for(auto& f : scenario_files) {
		if(f.ident.mod_path == mod_path) {
			selected_scenario_file = f.file_name;
			break;
		}
	}
	file_is_ready.store(true, std::memory_order::memory_order_release);
}

void mouse_click() {
	if(obj_under_mouse == -1)
		return;

	switch(obj_under_mouse) {
	case ui_obj_close:
		PostMessageW(m_hwnd, WM_CLOSE, 0, 0);
		return;
	case ui_obj_list_left:
		if(frame_in_list > 0) {
			--frame_in_list;
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}
		return;
	case ui_obj_list_right:
		if((frame_in_list + 1) * ui_list_count < int32_t(mod_list.size())) {
			++frame_in_list;
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}
		return;
	case ui_obj_create_scenario:
		if(file_is_ready.load(std::memory_order::memory_order_acquire)) {
			make_mod_file();
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}
		return;
	case ui_obj_play_game:
		if(file_is_ready.load(std::memory_order::memory_order_acquire) && !selected_scenario_file.empty()) {
			if(IsProcessorFeaturePresent(PF_AVX512F_INSTRUCTIONS_AVAILABLE)) {
				native_string temp_command_line = native_string(NATIVE("Alice512.exe ")) + selected_scenario_file;

				STARTUPINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				PROCESS_INFORMATION pi;
				ZeroMemory(&pi, sizeof(pi));
				// Start the child process.
				if(CreateProcessW(
					nullptr,   // Module name
					const_cast<wchar_t*>(temp_command_line.c_str()), // Command line
					nullptr, // Process handle not inheritable
					nullptr, // Thread handle not inheritable
					FALSE, // Set handle inheritance to FALSE
					0, // No creation flags
					nullptr, // Use parent's environment block
					nullptr, // Use parent's starting directory
					&si, // Pointer to STARTUPINFO structure
					&pi) != 0) {

					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);

					return; // exit -- don't try starting avx2
				}
			}
			if(!IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE)) {
				native_string temp_command_line = native_string(NATIVE("AliceSSE.exe ")) + selected_scenario_file;

				STARTUPINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				PROCESS_INFORMATION pi;
				ZeroMemory(&pi, sizeof(pi));
				// Start the child process.
				if(CreateProcessW(
					nullptr,   // Module name
					const_cast<wchar_t*>(temp_command_line.c_str()), // Command line
					nullptr, // Process handle not inheritable
					nullptr, // Thread handle not inheritable
					FALSE, // Set handle inheritance to FALSE
					0, // No creation flags
					nullptr, // Use parent's environment block
					nullptr, // Use parent's starting directory
					&si, // Pointer to STARTUPINFO structure
					&pi) != 0) {

					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);

					return; // exit -- don't try starting avx2
				}
			}
			{ // normal case (avx2)
				native_string temp_command_line = native_string(NATIVE("Alice.exe ")) + selected_scenario_file;

				STARTUPINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				PROCESS_INFORMATION pi;
				ZeroMemory(&pi, sizeof(pi));
				// Start the child process.
				if(CreateProcessW(
					nullptr,   // Module name
					const_cast<wchar_t*>(temp_command_line.c_str()), // Command line
					nullptr, // Process handle not inheritable
					nullptr, // Thread handle not inheritable
					FALSE, // Set handle inheritance to FALSE
					0, // No creation flags
					nullptr, // Use parent's environment block
					nullptr, // Use parent's starting directory
					&si, // Pointer to STARTUPINFO structure
					&pi) != 0) {

					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);
				}
				return;
			}
		}
	case ui_obj_host_game:
		[[fallthrough]];
	case ui_obj_join_game:
		if(file_is_ready.load(std::memory_order::memory_order_acquire) && !selected_scenario_file.empty()) {
			native_string temp_command_line = native_string(NATIVE("AliceSSE.exe ")) + selected_scenario_file;
			if(obj_under_mouse == ui_obj_host_game) {
				temp_command_line += NATIVE(" -host");
				temp_command_line += NATIVE(" -name ");
				temp_command_line += simple_fs::utf8_to_native(player_name.to_string());

				if (!player_password.empty()) {
					temp_command_line += NATIVE(" -player_password ");
					temp_command_line += simple_fs::utf8_to_native(player_password.to_string());
				}

			} else if(obj_under_mouse == ui_obj_join_game) {
				temp_command_line += NATIVE(" -join");
				temp_command_line += NATIVE(" ");
				temp_command_line += simple_fs::utf8_to_native(ip_addr);
				temp_command_line += NATIVE(" -name ");
				temp_command_line += simple_fs::utf8_to_native(player_name.to_string());

				if (!player_password.empty()) {
					temp_command_line += NATIVE(" -player_password ");
					temp_command_line += simple_fs::utf8_to_native(player_password.to_string());
				}

				// IPv6 address
				if(!ip_addr.empty() && ::strchr(ip_addr.c_str(), ':') != nullptr) {
					temp_command_line += NATIVE(" -v6");
				}
			}

			if(!lobby_password.empty()) {
				temp_command_line += NATIVE(" -password ");
				temp_command_line += simple_fs::utf8_to_native(lobby_password);
			}

			STARTUPINFO si;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));
			// Start the child process.
			if(CreateProcessW(
				nullptr,   // Module name
				const_cast<wchar_t*>(temp_command_line.c_str()), // Command line
				nullptr, // Process handle not inheritable
				nullptr, // Thread handle not inheritable
				FALSE, // Set handle inheritance to FALSE
				0, // No creation flags
				nullptr, // Use parent's environment block
				nullptr, // Use parent's starting directory
				&si, // Pointer to STARTUPINFO structure
				&pi) != 0) {

				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}


			// ready to launch
		}
		return;
	case ui_obj_ip_addr:
		return;
	case ui_obj_password:
		return;
	case ui_obj_player_name:
		return;
	default:
		break;
	}

	if(!file_is_ready.load(std::memory_order::memory_order_acquire))
		return;

	int32_t list_position = (obj_under_mouse - ui_list_first) / 3;
	int32_t sub_obj = (obj_under_mouse - ui_list_first) - list_position * 3;

	switch(sub_obj) {
	case ui_list_checkbox:
	{
		int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count + list_position;
		if(list_offset < int32_t(launcher::mod_list.size())) {
			launcher::mod_list[list_offset].mod_selected = !launcher::mod_list[list_offset].mod_selected;
			if(!launcher::mod_list[list_offset].mod_selected) {
				recursively_remove_from_list(launcher::mod_list[list_offset]);
			} else {
				recursively_add_to_list(launcher::mod_list[list_offset]);
			}
			enforce_list_order();
			find_scenario_file();
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}
		return;
	}
	case ui_list_move_up:
	{
		int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count + list_position;
		if(launcher::mod_list[list_offset].mod_selected && nth_item_can_move_up(list_offset)) {
			std::swap(launcher::mod_list[list_offset], launcher::mod_list[list_offset - 1]);
			find_scenario_file();
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}
		return;
	}
	case ui_list_move_down:
	{
		int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count + list_position;
		if(launcher::mod_list[list_offset].mod_selected && nth_item_can_move_down(list_offset)) {
			std::swap(launcher::mod_list[list_offset], launcher::mod_list[list_offset + 1]);
			find_scenario_file();
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}
		return;
	}
	default:
		break;
	}
}

GLint compile_shader(std::string_view source, GLenum type) {
	GLuint return_value = glCreateShader(type);

	if(return_value == 0) {
		MessageBoxW(m_hwnd, L"shader creation failed", L"OpenGL error", MB_OK);
	}

	std::string s_source(source);
	GLchar const* texts[] = {
		"#version 140\r\n",
		"#extension GL_ARB_explicit_uniform_location : enable\r\n",
		"#extension GL_ARB_explicit_attrib_location : enable\r\n",
		"#extension GL_ARB_shader_subroutine : enable\r\n",
		"#define M_PI 3.1415926535897932384626433832795\r\n",
		"#define PI 3.1415926535897932384626433832795\r\n",
		s_source.c_str()
	};
	glShaderSource(return_value, 7, texts, nullptr);
	glCompileShader(return_value);

	GLint result;
	glGetShaderiv(return_value, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		GLint log_length = 0;
		glGetShaderiv(return_value, GL_INFO_LOG_LENGTH, &log_length);

		auto log = std::unique_ptr<char[]>(new char[static_cast<size_t>(log_length)]);
		GLsizei written = 0;
		glGetShaderInfoLog(return_value, log_length, &written, log.get());
		auto error = std::string("Shader failed to compile:\n") + log.get();
		MessageBoxA(m_hwnd, error.c_str(), "OpenGL error", MB_OK);
	}
	return return_value;
}

GLuint create_program(std::string_view vertex_shader, std::string_view fragment_shader) {
	GLuint return_value = glCreateProgram();
	if(return_value == 0) {
		MessageBoxW(m_hwnd, L"program creation failed", L"OpenGL error", MB_OK);
	}

	auto v_shader = compile_shader(vertex_shader, GL_VERTEX_SHADER);
	auto f_shader = compile_shader(fragment_shader, GL_FRAGMENT_SHADER);

	glAttachShader(return_value, v_shader);
	glAttachShader(return_value, f_shader);
	glLinkProgram(return_value);

	GLint result;
	glGetProgramiv(return_value, GL_LINK_STATUS, &result);
	if(result == GL_FALSE) {
		GLint logLen;
		glGetProgramiv(return_value, GL_INFO_LOG_LENGTH, &logLen);

		char* log = new char[static_cast<size_t>(logLen)];
		GLsizei written;
		glGetProgramInfoLog(return_value, logLen, &written, log);
		auto err = std::string("Program failed to link:\n") + log;
		MessageBoxA(m_hwnd, err.c_str(), "OpenGL error", MB_OK);
	}

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	return return_value;
}

static GLfloat global_square_data[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f };
static GLfloat global_square_right_data[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
static GLfloat global_square_left_data[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat global_square_flipped_data[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat global_square_right_flipped_data[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
static GLfloat global_square_left_flipped_data[] = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

static GLuint ui_shader_program = 0;

void load_shaders() {
	simple_fs::file_system fs;
	simple_fs::add_root(fs, NATIVE("."));
	auto root = get_root(fs);

	std::string_view fx_str =
		"in vec2 tex_coord;\n"
		"out vec4 frag_color;\n"
		"uniform sampler2D texture_sampler;\n"
		"uniform vec4 d_rect;\n"
		"uniform float border_size;\n"
		"uniform vec3 inner_color;\n"
		"uniform vec4 subrect;\n"
		"uniform uvec2 subroutines_index;\n"
		"vec4 color_filter(vec2 tc) {\n"
		"\tvec4 color_in = texture(texture_sampler, tc);\n"
		"\tfloat sm_val = smoothstep(0.5 - border_size / 2.0, 0.5 + border_size / 2.0, color_in.r);\n"
		"\treturn vec4(inner_color, sm_val);\n"
		"}\n"
		"vec4 no_filter(vec2 tc) {\n"
		"\treturn texture(texture_sampler, tc);\n"
		"}\n"
		"vec4 disabled_color(vec4 color_in) {\n"
		"\tfloat amount = (color_in.r + color_in.g + color_in.b) / 4.0;\n"
		"\treturn vec4(amount, amount, amount, color_in.a);\n"
		"}\n"
		"vec4 interactable_color(vec4 color_in) {\n"
		"\treturn vec4(color_in.r + 0.1, color_in.g + 0.1, color_in.b + 0.1, color_in.a);\n"
		"}\n"
		"vec4 interactable_disabled_color(vec4 color_in) {\n"
		"\tfloat amount = (color_in.r + color_in.g + color_in.b) / 4.0;\n"
		"\treturn vec4(amount + 0.1, amount + 0.1, amount + 0.1, color_in.a);\n"
		"}\n"
		"vec4 tint_color(vec4 color_in) {\n"
		"\treturn vec4(color_in.r * inner_color.r, color_in.g * inner_color.g, color_in.b * inner_color.b, color_in.a);\n"
		"}\n"
		"vec4 enabled_color(vec4 color_in) {\n"
		"\treturn color_in;\n"
		"}\n"
		"vec4 alt_tint_color(vec4 color_in) {\n"
		"\treturn vec4(color_in.r * subrect.r, color_in.g * subrect.g, color_in.b * subrect.b, color_in.a);\n"
		"}\n"
		"vec4 font_function(vec2 tc) {\n"
		"\treturn int(subroutines_index.y) == 1 ? color_filter(tc) : no_filter(tc);\n"
		"}\n"
		"vec4 coloring_function(vec4 tc) {\n"
		"\tswitch(int(subroutines_index.x)) {\n"
		"\tcase 3: return disabled_color(tc);\n"
		"\tcase 4: return enabled_color(tc);\n"
		"\tcase 12: return tint_color(tc);\n"
		"\tcase 13: return interactable_color(tc);\n"
		"\tcase 14: return interactable_disabled_color(tc);\n"
		"\tcase 16: return alt_tint_color(tc);\n"
		"\tdefault: break;\n"
		"\t}\n"
		"\treturn tc;\n"
		"}\n"
		"void main() {\n"
		"\tfrag_color = coloring_function(font_function(tex_coord));\n"
		"}";
	std::string_view vx_str =
		"layout (location = 0) in vec2 vertex_position;\n"
		"layout (location = 1) in vec2 v_tex_coord;\n"
		"out vec2 tex_coord;\n"
		"uniform float screen_width;\n"
		"uniform float screen_height;\n"
		"uniform vec4 d_rect;\n"
		"void main() {\n"
		"\tgl_Position = vec4(\n"
		"\t\t-1.0 + (2.0 * ((vertex_position.x * d_rect.z)  + d_rect.x) / screen_width),\n"
		"\t\t 1.0 - (2.0 * ((vertex_position.y * d_rect.w)  + d_rect.y) / screen_height),\n"
		"\t\t0.0, 1.0);\n"
		"\ttex_coord = v_tex_coord;\n"
		"}";

	ui_shader_program = create_program(vx_str, fx_str);
}

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

void render() {
	if(!opengl_context)
		return;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(ui_shader_program);
	glUniform1i(glGetUniformLocation(ui_shader_program, "texture_sampler"), 0);
	glUniform1f(glGetUniformLocation(ui_shader_program, "screen_width"), float(base_width));
	glUniform1f(glGetUniformLocation(ui_shader_program, "screen_height"), float(base_height));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, int32_t(base_width * scaling_factor), int32_t(base_height * scaling_factor));
	glDepthRange(-1.0f, 1.0f);

	launcher::ogl::render_textured_rect(launcher::ogl::color_modification::none, 0, 0, int32_t(base_width), int32_t(base_height), bg_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_new_text("Project Alice", launcher::ogl::color_modification::none, 83, 5, 26, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[1]);

	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_close ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_close].x,
		ui_rects[ui_obj_close].y,
		ui_rects[ui_obj_close].width,
		ui_rects[ui_obj_close].height,
		close_tex.get_texture_handle(), ui::rotation::upright, false);

	if(int32_t(mod_list.size()) > ui_list_count) {
		if(frame_in_list > 0) {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_list_left ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
				ui_rects[ui_obj_list_left].x,
				ui_rects[ui_obj_list_left].y,
				ui_rects[ui_obj_list_left].width,
				ui_rects[ui_obj_list_left].height,
				left_tex.get_texture_handle(), ui::rotation::upright, false);
		} else {
			launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
				ui_rects[ui_obj_list_left].x,
				ui_rects[ui_obj_list_left].y,
				ui_rects[ui_obj_list_left].width,
				ui_rects[ui_obj_list_left].height,
				left_tex.get_texture_handle(), ui::rotation::upright, false);
		}

		if((frame_in_list + 1) * ui_list_count < int32_t(mod_list.size())) {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_list_right ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
				ui_rects[ui_obj_list_right].x,
				ui_rects[ui_obj_list_right].y,
				ui_rects[ui_obj_list_right].width,
				ui_rects[ui_obj_list_right].height,
				right_tex.get_texture_handle(), ui::rotation::upright, false);
		} else {
			launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
				ui_rects[ui_obj_list_right].x,
				ui_rects[ui_obj_list_right].y,
				ui_rects[ui_obj_list_right].width,
				ui_rects[ui_obj_list_right].height,
				right_tex.get_texture_handle(), ui::rotation::upright, false);
		}
	}

	if(file_is_ready.load(std::memory_order::memory_order_acquire)) {
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_create_scenario ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_create_scenario].x,
			ui_rects[ui_obj_create_scenario].y,
			ui_rects[ui_obj_create_scenario].width,
			ui_rects[ui_obj_create_scenario].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		if(game_dir_not_found) {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_create_scenario ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
				ui_rects[ui_obj_create_scenario].x,
				ui_rects[ui_obj_create_scenario].y,
				44,
				33,
				warning_tex.get_texture_handle(), ui::rotation::upright, false);
		}

		if(selected_scenario_file.empty()) {
			auto sv = launcher::localised_strings[uint8_t(launcher::string_index::create_scenario)];
			float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
			launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, ui_rects[ui_obj_create_scenario].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);
		} else {
			auto sv = launcher::localised_strings[uint8_t(launcher::string_index::recreate_scenario)];
			float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
			launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, ui_rects[ui_obj_create_scenario].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);
		}
	} else {
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_create_scenario].x,
			ui_rects[ui_obj_create_scenario].y,
			ui_rects[ui_obj_create_scenario].width,
			ui_rects[ui_obj_create_scenario].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		if(game_dir_not_found) {
			launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
				ui_rects[ui_obj_create_scenario].x,
				ui_rects[ui_obj_create_scenario].y,
				44,
				33,
				warning_tex.get_texture_handle(), ui::rotation::upright, false);
		}
		auto sv = launcher::localised_strings[uint8_t(launcher::string_index::working)];
		float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, 50.0f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);
	}

	{
		// Create a new scenario file for the selected mods
		auto sv = launcher::localised_strings[uint8_t(launcher::string_index::create_a_new_scenario)];
		auto xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, 94.0f + 0 * 18.0f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
		sv = launcher::localised_strings[uint8_t(launcher::string_index::for_the_selected_mods)];
		xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, 94.0f + 1 * 18.0f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	}

	if(file_is_ready.load(std::memory_order::memory_order_acquire) && !selected_scenario_file.empty()) {
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_play_game ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_play_game].x,
			ui_rects[ui_obj_play_game].y,
			ui_rects[ui_obj_play_game].width,
			ui_rects[ui_obj_play_game].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_host_game ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_host_game].x,
			ui_rects[ui_obj_host_game].y,
			ui_rects[ui_obj_host_game].width,
			ui_rects[ui_obj_host_game].height,
			big_l_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_join_game ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_join_game].x,
			ui_rects[ui_obj_join_game].y,
			ui_rects[ui_obj_join_game].width,
			ui_rects[ui_obj_join_game].height,
			big_r_button_tex.get_texture_handle(), ui::rotation::upright, false);
	} else {
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_play_game].x,
			ui_rects[ui_obj_play_game].y,
			ui_rects[ui_obj_play_game].width,
			ui_rects[ui_obj_play_game].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_host_game].x,
			ui_rects[ui_obj_host_game].y,
			ui_rects[ui_obj_host_game].width,
			ui_rects[ui_obj_host_game].height,
			big_l_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_join_game].x,
			ui_rects[ui_obj_join_game].y,
			ui_rects[ui_obj_join_game].width,
			ui_rects[ui_obj_join_game].height,
			big_r_button_tex.get_texture_handle(), ui::rotation::upright, false);

		/*830, 250*/
		// No scenario file found

		auto sv = launcher::localised_strings[uint8_t(launcher::string_index::no_scenario_found)];
		auto xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, ui_rects[ui_obj_play_game].y + 48.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	}

	auto sv = launcher::localised_strings[uint8_t(launcher::string_index::ip_address)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_ip_addr].x + ui_rects[ui_obj_ip_addr].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]), ui_rects[ui_obj_ip_addr].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_ip_addr ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_ip_addr].x,
		ui_rects[ui_obj_ip_addr].y,
		ui_rects[ui_obj_ip_addr].width,
		ui_rects[ui_obj_ip_addr].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::lobby_password)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_password].x + ui_rects[ui_obj_password].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]), ui_rects[ui_obj_password].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_password ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_password].x,
		ui_rects[ui_obj_password].y,
		ui_rects[ui_obj_password].width,
		ui_rects[ui_obj_password].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::nickname)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_player_name].x + ui_rects[ui_obj_player_name].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]), ui_rects[ui_obj_player_name].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_player_name ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_player_name].x,
		ui_rects[ui_obj_player_name].y,
		ui_rects[ui_obj_player_name].width,
		ui_rects[ui_obj_player_name].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::player_password)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_player_password].x + ui_rects[ui_obj_player_password].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]), ui_rects[ui_obj_player_password].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_player_password ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_player_password].x,
		ui_rects[ui_obj_player_password].y,
		ui_rects[ui_obj_player_password].width,
		ui_rects[ui_obj_player_password].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::singleplayer)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_play_game].x + ui_rects[ui_obj_play_game].width - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]), ui_rects[ui_obj_play_game].y - 32.f, 22.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[1]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::start_game)];
	float sg_x_pos = ui_rects[ui_obj_play_game].x + ui_rects[ui_obj_play_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, sg_x_pos, ui_rects[ui_obj_play_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::multiplayer)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_join_game].x + ui_rects[ui_obj_join_game].width - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]), ui_rects[ui_obj_host_game].y - 32.f, 22.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[1]);

	// Join and host game buttons
	sv = launcher::localised_strings[uint8_t(launcher::string_index::host)];
	float hg_x_pos = ui_rects[ui_obj_host_game].x + ui_rects[ui_obj_host_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, hg_x_pos, ui_rects[ui_obj_host_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);
	sv = launcher::localised_strings[uint8_t(launcher::string_index::join)];
	float jg_x_pos = ui_rects[ui_obj_join_game].x + ui_rects[ui_obj_join_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, jg_x_pos, ui_rects[ui_obj_join_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);

	// Text fields
	float ia_x_pos = ui_rects[ui_obj_ip_addr].x + 6.f;// ui_rects[ui_obj_ip_addr].width - base_text_extent(ip_addr.c_str(), uint32_t(ip_addr.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(ip_addr.c_str(), launcher::ogl::color_modification::none, ia_x_pos, ui_rects[ui_obj_ip_addr].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, fonts[0]);
	float ps_x_pos = ui_rects[ui_obj_password].x + 6.f;
	launcher::ogl::render_new_text(lobby_password.c_str(), launcher::ogl::color_modification::none, ia_x_pos, ui_rects[ui_obj_password].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, fonts[0]);
	float pn_x_pos = ui_rects[ui_obj_player_name].x + 6.f;// ui_rects[ui_obj_player_name].width - base_text_extent(player_name.c_str(), uint32_t(player_name.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(player_name.to_string_view(), launcher::ogl::color_modification::none, pn_x_pos, ui_rects[ui_obj_player_name].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, fonts[0]);
	float pp_x_pos = ui_rects[ui_obj_player_password].x + 6.f;// ui_rects[ui_obj_player_password].width - base_text_extent(player_name.c_str(), uint32_t(player_name.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(player_password.to_string_view(), launcher::ogl::color_modification::none, pn_x_pos, ui_rects[ui_obj_player_password].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, fonts[0]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::mod_list)];
	auto ml_xoffset = list_text_right_align - base_text_extent(sv.data(), uint32_t(sv.size()), 24, fonts[1]);
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ml_xoffset, 45.0f, 24.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[1]);

	int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count;

	for(int32_t i = 0; i < ui_list_count && list_offset + i < int32_t(mod_list.size()); ++i) {
		auto& mod_ref = mod_list[list_offset + i];

		if(i % 2 == 1) {
			launcher::ogl::render_textured_rect(
				launcher::ogl::color_modification::none,
				60,
				75 + ui_row_height * i,
				440,
				ui_row_height,
				launcher::line_bg_tex.get_texture_handle(), ui::rotation::upright, false);
		}

		if(mod_ref.mod_selected) {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_checkbox ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].x,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].y,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].width,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].height,
			check_tex.get_texture_handle(), ui::rotation::upright, false);

			if(!nth_item_can_move_up(list_offset + i)) {
				launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].x,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].y,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].width,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].height,
					up_tex.get_texture_handle(), ui::rotation::upright, false);
			} else {
				launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_move_up ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].x,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].y,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].width,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].height,
					up_tex.get_texture_handle(), ui::rotation::upright, false);
			}
			if(!nth_item_can_move_down(list_offset + i)) {
				launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].x,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].y,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].width,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].height,
					down_tex.get_texture_handle(), ui::rotation::upright, false);
			} else {
				launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_move_down ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].x,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].y,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].width,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].height,
					down_tex.get_texture_handle(), ui::rotation::upright, false);
			}
		} else {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_checkbox ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].x,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].y,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].width,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].height,
			empty_check_tex.get_texture_handle(), ui::rotation::upright, false);
		}

		auto xoffset = list_text_right_align - base_text_extent(mod_ref.name_.data(), uint32_t(mod_ref.name_.length()), 14, fonts[0]);

		launcher::ogl::render_new_text(mod_ref.name_.data(), launcher::ogl::color_modification::none, xoffset, 75.0f + 7.0f + i * ui_row_height, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	}

	SwapBuffers(opengl_window_dc);
}

bool is_low_surrogate(uint16_t char_code) noexcept {
	return char_code >= 0xDC00 && char_code <= 0xDFFF;
}
bool is_high_surrogate(uint16_t char_code) noexcept {
	return char_code >= 0xD800 && char_code <= 0xDBFF;
}

char process_utf16_to_win1250(wchar_t c) {
	if(c <= 127)
		return char(c);
	if(is_low_surrogate(c) || is_high_surrogate(c))
		return 0;
	char char_out = 0;
	WideCharToMultiByte(1250, 0, &c, 1, &char_out, 1, nullptr, nullptr);
	return char_out;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if(message == WM_CREATE) {
		opengl_window_dc = GetDC(hwnd);

		create_opengl_context();

		glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
		glEnable(GL_LINE_SMOOTH);

		load_shaders();				// create shaders
		load_global_squares();		// create various squares to drive the shaders with

		simple_fs::file_system fs;
		simple_fs::add_root(fs, NATIVE("."));
		auto root = get_root(fs);

		uint8_t font_set_load = 0;
		LANGID lang = GetUserDefaultUILanguage();
		//lang = 0x0004;
		switch(lang & 0xff) {
		//case 0x0001:
		//	localised_strings = &ar_localised_strings[0];
		//	font_set_load = 2;
		//	break;
		case 0x0002:
			localised_strings = &bg_localised_strings[0];
			font_set_load = 3;
			break;
		case 0x0003:
			localised_strings = &ca_localised_strings[0];
			break;
		case 0x0004:
			localised_strings = &zh_localised_strings[0];
			font_set_load = 1;
			break;
		case 0x0005:
			localised_strings = &cs_localised_strings[0];
			break;
		case 0x0006:
			localised_strings = &da_localised_strings[0];
			break;
		case 0x0007:
			localised_strings = &de_localised_strings[0];
			break;
		case 0x0008:
			localised_strings = &el_localised_strings[0];
			break;
		case 0x0009:
			localised_strings = &en_localised_strings[0];
			break;
		case 0x000A:
			localised_strings = &es_localised_strings[0];
			break;
		case 0x000B:
			localised_strings = &fi_localised_strings[0];
			break;
		case 0x000C:
			localised_strings = &fr_localised_strings[0];
			break;
		//case 0x000D:
		//	localised_strings = &he_localised_strings[0];
		//	break;
		case 0x000E:
			localised_strings = &hu_localised_strings[0];
			break;
		case 0x000F:
			//localised_strings = &is_localised_strings[0];
			break;
		case 0x0010:
			localised_strings = &it_localised_strings[0];
			break;
		case 0x0011:
			//localised_strings = &ja_localised_strings[0];
			break;
		case 0x0012:
			//localised_strings = &ko_localised_strings[0];
			break;
		case 0x0016:
			localised_strings = &po_localised_strings[0];
			break;
		case 0x0013:
			localised_strings = &nl_localised_strings[0];
			break;
		case 0x0014:
			localised_strings = &no_localised_strings[0];
			break;
		case 0x0015:
			localised_strings = &pl_localised_strings[0];
			break;
		case 0x0018:
			localised_strings = &ro_localised_strings[0];
			break;
		case 0x0019:
			localised_strings = &ru_localised_strings[0];
			font_set_load = 3;
			break;
		case 0x001C:
			localised_strings = &sq_localised_strings[0];
			break;
		case 0x001D:
			localised_strings = &sv_localised_strings[0];
			break;
		case 0x001F:
			localised_strings = &tr_localised_strings[0];
			break;
		case 0x0022:
			localised_strings = &uk_localised_strings[0];
			font_set_load = 3;
			break;
		case 0x0025:
			localised_strings = &et_localised_strings[0];
			break;
		case 0x0026:
			localised_strings = &lv_localised_strings[0];
			break;
		case 0x0027:
			localised_strings = &lt_localised_strings[0];
			break;
		case 0x002A:
			localised_strings = &vi_localised_strings[0];
			break;
		case 0x002B:
			localised_strings = &hy_localised_strings[0];
			break;
		case 0x0039:
			localised_strings = &hi_localised_strings[0];
			break;
		default:
			break;
		}
		if(font_set_load == 0) {
			auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/LibreCaslonText-Regular.ttf"));
			if(font_a) {
				auto file_content = simple_fs::view_contents(*font_a);
				font_collection.load_font(fonts[0], file_content.data, file_content.file_size);
			}
			auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/LibreCaslonText-Italic.ttf"));
			if(font_b) {
				auto file_content = simple_fs::view_contents(*font_b);
				font_collection.load_font(fonts[1], file_content.data, file_content.file_size);
			}
		} else if(font_set_load == 1) { //chinese
			auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/STZHONGS.TTF"));
			if(font_a) {
				auto file_content = simple_fs::view_contents(*font_a);
				font_collection.load_font(fonts[0], file_content.data, file_content.file_size);
			}
			auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/STZHONGS.TTF"));
			if(font_b) {
				auto file_content = simple_fs::view_contents(*font_b);
				font_collection.load_font(fonts[1], file_content.data, file_content.file_size);
			}
		} else if(font_set_load == 2) { //arabic
			auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/NotoNaskhArabic-Bold.ttf"));
			if(font_a) {
				auto file_content = simple_fs::view_contents(*font_a);
				font_collection.load_font(fonts[0], file_content.data, file_content.file_size);
			}
			auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/NotoNaskhArabic-Regular.ttf"));
			if(font_b) {
				auto file_content = simple_fs::view_contents(*font_b);
				font_collection.load_font(fonts[1], file_content.data, file_content.file_size);
			}
		} else if(font_set_load == 3) { //cyrillic
			auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/NotoSerif-Regular.ttf"));
			if(font_a) {
				auto file_content = simple_fs::view_contents(*font_a);
				font_collection.load_font(fonts[0], file_content.data, file_content.file_size);
			}
			auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/NotoSerif-Regular.ttf"));
			if(font_b) {
				auto file_content = simple_fs::view_contents(*font_b);
				font_collection.load_font(fonts[1], file_content.data, file_content.file_size);
			}
		}

		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_bg.png"), fs, bg_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_left.png"), fs, left_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_right.png"), fs, right_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_close.png"), fs, close_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_big_button.png"), fs, big_button_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_big_left.png"), fs, big_l_button_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_big_right.png"), fs, big_r_button_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_no_check.png"), fs, empty_check_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_check.png"), fs, check_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_up.png"), fs, up_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_down.png"), fs, down_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_line_bg.png"), fs, line_bg_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_warning.png"), fs, warning_tex, false);

		game_dir_not_found = false;
		{
			auto f = simple_fs::peek_file(root, NATIVE("v2game.exe"));
			if(!f) {
				f = simple_fs::peek_file(root, NATIVE("victoria2.exe"));
				if(!f) {
					game_dir_not_found = true;
				}
			}
		}

		// Find all mods
		auto mod_dir = simple_fs::open_directory(root, NATIVE("mod"));
		auto mod_files = simple_fs::list_files(mod_dir, NATIVE(".mod"));

		parsers::error_handler err("");
		for(auto& f : mod_files) {
			auto of = simple_fs::open_file(f);
			if(of) {
				auto content = view_contents(*of);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				mod_list.push_back(parsers::parse_mod_file(gen, err, parsers::mod_file_context{}));
			}
		}

		// Find all scenario files
		auto sdir = simple_fs::get_or_create_scenario_directory();
		auto s_files = simple_fs::list_files(sdir, NATIVE(".bin"));
		for(auto& f : s_files) {
			auto of = simple_fs::open_file(f);
			if(of) {
				auto content = view_contents(*of);
				auto desc = sys::extract_mod_information(reinterpret_cast<uint8_t const*>(content.data), content.file_size);
				if(desc.count != 0) {
					max_scenario_count = std::max(desc.count, max_scenario_count);
					scenario_files.push_back(scenario_file{ simple_fs::get_file_name(f) , desc });
				}
			}
		}

		std::sort(scenario_files.begin(), scenario_files.end(), [](scenario_file const& a, scenario_file const& b) {
			return a.ident.count > b.ident.count;
		});

		// Process command line arguments
		int num_params = 0;
		auto parsed_cmd = CommandLineToArgvW(GetCommandLineW(), &num_params);
		for(int i = 1; i < num_params; ++i) {
			if(native_string(parsed_cmd[i]) == NATIVE("-outputScenario")) {
				auto str = simple_fs::native_to_utf8(native_string(parsed_cmd[i + 1]));
				requestedScenarioFileName = str;
			}
			if(native_string(parsed_cmd[i]) == NATIVE("-modsMask")) {
				auto str = simple_fs::native_to_utf8(native_string(parsed_cmd[i + 1]));
				enabledModsMask = str;
			}
			if(native_string(parsed_cmd[i]) == NATIVE("-autoBuild")) {
				autoBuild = true;
			}
		}

		find_scenario_file();

		if(enabledModsMask != "") {
			for(auto& mod : launcher::mod_list) {
				if(mod.name_.find(enabledModsMask) != std::string::npos) {
					mod.mod_selected = true;

					recursively_add_to_list(mod);
					enforce_list_order();
					find_scenario_file();
					InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
				}
			}
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}

		if(autoBuild && file_is_ready.load(std::memory_order::memory_order_acquire)) {
			make_mod_file();
			find_scenario_file();
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}

		find_scenario_file();

		return 1;
	} else {
		switch(message) {
		case WM_DPICHANGED:
		{
			dpi = float(HIWORD(wParam));
			auto lprcNewScale = reinterpret_cast<RECT*>(lParam);


			scaling_factor = float(lprcNewScale->right - lprcNewScale->left) / base_width;

			SetWindowPos(hwnd, nullptr, lprcNewScale->left, lprcNewScale->top,
				lprcNewScale->right - lprcNewScale->left, lprcNewScale->bottom - lprcNewScale->top,
				SWP_NOZORDER | SWP_NOACTIVATE);

			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);

			break;
		}
		case WM_NCMOUSEMOVE:
		{
			RECT rcWindow;
			GetWindowRect(hwnd, &rcWindow);
			auto x = GET_X_LPARAM(lParam);
			auto y = GET_Y_LPARAM(lParam);

			POINTS adj{ SHORT(x - rcWindow.left), SHORT(y - rcWindow.top) };
			memcpy(&lParam, &adj, sizeof(LPARAM));

			mouse_x = int32_t(float(GET_X_LPARAM(lParam)) / scaling_factor);
			mouse_y = int32_t(float(GET_Y_LPARAM(lParam)) / scaling_factor);
			if(update_under_mouse()) {
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			mouse_x = int32_t(float(GET_X_LPARAM(lParam)) / scaling_factor);
			mouse_y = int32_t(float(GET_Y_LPARAM(lParam)) / scaling_factor);
			if(update_under_mouse()) {
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			mouse_click();
			return 0;
		}
		case WM_NCCALCSIZE:
			if(wParam == TRUE)
				return 0;
			break;
		case WM_NCHITTEST:
		{
			POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			RECT rcWindow;
			GetWindowRect(hwnd, &rcWindow);

			if(ptMouse.x <= int32_t(rcWindow.left + caption_width * scaling_factor)
				&& ptMouse.y <= int32_t(rcWindow.top + caption_height * scaling_factor)) {

				return HTCAPTION;
			} else {
				return HTCLIENT;
			}
		}
		case WM_PAINT:
		case WM_DISPLAYCHANGE:
		{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);

			render();

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 1;
		case WM_KEYDOWN:
			if(GetKeyState(VK_CONTROL) & 0x8000) {
				if(wParam == L'v' || wParam == L'V') {
					if(!IsClipboardFormatAvailable(CF_TEXT))
						return 0;
					if(!OpenClipboard(m_hwnd))
						return 0;

					auto hglb = GetClipboardData(CF_TEXT);
					if(hglb != nullptr) {
						auto lptstr = GlobalLock(hglb);
						if(lptstr != nullptr) {
							std::string cb_data((char*)lptstr);
							while(cb_data.length() > 0 && isspace(cb_data.back())) {
								cb_data.pop_back();
							}
							ip_addr = cb_data;
							GlobalUnlock(hglb);
						}
					}
					CloseClipboard();
				}
			}
			return 0;
		case WM_CHAR:
		{
			if(GetKeyState(VK_CONTROL) & 0x8000) {

			} else {
				char turned_into = process_utf16_to_win1250(wchar_t(wParam));
				if(turned_into) {
					if(obj_under_mouse == ui_obj_ip_addr) {
						if(turned_into == '\b') {
							if(!ip_addr.empty())
								ip_addr.pop_back();
						} else if(turned_into >= 32 && turned_into != '\t' && turned_into != ' ' && ip_addr.size() < 46) {
							ip_addr.push_back(turned_into);
						}
					} else if(obj_under_mouse == ui_obj_player_name) {
						if(turned_into == '\b') {
							if(!player_name.empty()) {
								player_name.pop();
								save_playername();
							}
						} else if(turned_into >= 32 && turned_into != '\t' && turned_into != ' ') {
							player_name.append(turned_into);
							save_playername();
						}
					}
					else if (obj_under_mouse == ui_obj_player_password) {
						if (turned_into == '\b') {
							if (!player_password.empty()) {
								player_password.pop();
								save_playerpassw();
							}
						}
						else if (turned_into >= 32 && turned_into != '\t' && turned_into != ' ') {
							player_password.append(turned_into);
							save_playerpassw();
						}
					}
					else if(obj_under_mouse == ui_obj_password) {
						if(turned_into == '\b') {
							if(!lobby_password.empty())
								lobby_password.pop_back();
						} else if(turned_into >= 32 && turned_into != '\t' && turned_into != ' ' && lobby_password.size() < 16) {
							lobby_password.push_back(turned_into);
						}
					}
				}
			}
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			return 0;
		}
		default:
			break;

		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}

} // end launcher namespace

static CRITICAL_SECTION guard_abort_handler;

void signal_abort_handler(int) {
	static bool run_once = false;

	EnterCriticalSection(&guard_abort_handler);
	if(run_once == false) {
		run_once = true;

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		// Start the child process.
		if(CreateProcessW(
			L"dbg_alice.exe",   // Module name
			NULL, // Command line
			NULL, // Process handle not inheritable
			NULL, // Thread handle not inheritable
			FALSE, // Set handle inheritance to FALSE
			0, // No creation flags
			NULL, // Use parent's environment block
			NULL, // Use parent's starting directory
			&si, // Pointer to STARTUPINFO structure
			&pi) == 0) {

			// create process failed
			LeaveCriticalSection(&guard_abort_handler);
			return;

		}
		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Close process and thread handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	LeaveCriticalSection(&guard_abort_handler);
}

LONG WINAPI uef_wrapper(struct _EXCEPTION_POINTERS* lpTopLevelExceptionFilter) {
	signal_abort_handler(0);
	return EXCEPTION_CONTINUE_SEARCH;
}

void generic_wrapper() {
	signal_abort_handler(0);
}
void invalid_parameter_wrapper(
   const wchar_t* expression,
   const wchar_t* function,
   const wchar_t* file,
   unsigned int line,
   uintptr_t pReserved
) {
	signal_abort_handler(0);
}

void EnableCrashingOnCrashes() {
	typedef BOOL(WINAPI* tGetPolicy)(LPDWORD lpFlags);
	typedef BOOL(WINAPI* tSetPolicy)(DWORD dwFlags);
	const DWORD EXCEPTION_SWALLOWING = 0x1;

	HMODULE kernel32 = LoadLibraryA("kernel32.dll");
	tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32, "GetProcessUserModeExceptionPolicy");
	tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32, "SetProcessUserModeExceptionPolicy");
	if(pGetPolicy && pSetPolicy) {
		DWORD dwFlags;
		if(pGetPolicy(&dwFlags)) {
			// Turn off the filter
			pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
		}
	}
	BOOL insanity = FALSE;
	SetUserObjectInformationA(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &insanity, sizeof(insanity));
}

int WINAPI wWinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPWSTR /*lpCmdLine*/,
	int /*nCmdShow*/
) {
#ifdef _DEBUG
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif

	InitializeCriticalSection(&guard_abort_handler);

	if(!IsDebuggerPresent()) {
		EnableCrashingOnCrashes();
		_set_purecall_handler(generic_wrapper);
		_set_invalid_parameter_handler(invalid_parameter_wrapper);
		_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		SetUnhandledExceptionFilter(uef_wrapper);
		signal(SIGABRT, signal_abort_handler);
	}

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	if(!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		return 0;

	WNDCLASSEX wcex = { };
	wcex.cbSize = UINT(sizeof(WNDCLASSEX));
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = launcher::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.hIcon = (HICON)LoadImage(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = NATIVE("alice_launcher_class");

	if(RegisterClassEx(&wcex) == 0) {
		window::emit_error_message("Unable to register window class", true);
	}

	// Load from user settings
	launcher::load_playername();
	launcher::load_playerpassw();

	launcher::m_hwnd = CreateWindowEx(
		0,
		L"alice_launcher_class",
		L"Launch Project Alice",
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		NULL,
		NULL,
		GetModuleHandle(nullptr),
		nullptr
	);

	if(launcher::m_hwnd) {

		launcher::dpi = float(GetDpiForWindow((HWND)(launcher::m_hwnd)));

		auto monitor_handle = MonitorFromWindow((HWND)(launcher::m_hwnd), MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(monitor_handle, &mi);

		auto vert_space = mi.rcWork.bottom - mi.rcWork.top;
		float rough_scale = float(vert_space) / 1080.0f;
		if(rough_scale >= 1.0f) {
			launcher::scaling_factor = std::round(rough_scale);
		} else {
			launcher::scaling_factor = std::round(rough_scale * 4.0f) / 4.0f;
		}

		SetWindowPos(
			(HWND)(launcher::m_hwnd),
			NULL,
			NULL,
			NULL,
			static_cast<int>(launcher::scaling_factor * launcher::base_width),
			static_cast<int>(launcher::scaling_factor * launcher::base_height),
			SWP_NOMOVE | SWP_FRAMECHANGED);


		ShowWindow((HWND)(launcher::m_hwnd), SW_SHOWNORMAL);
		UpdateWindow((HWND)(launcher::m_hwnd));
	}

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return 0;
}
