#define _CRT_SECURE_NO_WARNINGS
#include "GUI.h"
#include <SFML/Graphics.hpp>
#include <fstream>
using namespace std;

//adding colours
static const sf::Color C_BG(245, 247, 250);
static const sf::Color C_HDR(25, 75, 145);
static const sf::Color C_BTN(50, 110, 180);
static const sf::Color C_HOV(80, 150, 220);
static const sf::Color C_RED(170, 40, 40);
static const sf::Color C_GREEN(35, 140, 70);
static const sf::Color C_GRAY(110, 110, 110);
static const sf::Color C_TEXT(25, 25, 25);
static const sf::Color C_WHITE(255, 255, 255);
static const sf::Color C_ROW_A(255, 255, 255);
static const sf::Color C_ROW_B(232, 241, 253);
static const sf::Color C_SEP(190, 205, 220);

//utilities
void GUI::cpy(char* d, const char* s) const
{
    int i = 0;
    while (s[i])
    { 
        d[i] = s[i]; i++;
    } 
    d[i] = '\0';
}

void GUI::cat(char* d, const char* s) const
{
    int i = clen(d), j = 0; 
    while (s[j])
    { 
        d[i++] = s[j++];
    } 
    d[i] = '\0';
}

int GUI::clen(const char* s) const
{
    int i = 0; 
    while (s[i])
    {
        i++;
    }
    return i;
}

bool GUI::ceq(const char* a, const char* b) const
{
    int i = 0;
    while (a[i] && b[i])
    { 
        if (a[i] != b[i])
        {
            return false;
        }
        i++;
    } 
    return a[i] == '\0' && b[i] == '\0';
}

bool GUI::ceqCI(const char* a, const char* b) const
{
    int i = 0;
    while (a[i] && b[i]) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return false; i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

void GUI::istr(int n, char* b) const
{
    if (n == 0) { b[0] = '0'; b[1] = '\0'; return; }
    int i = 0, t = n; while (t > 0) { t /= 10; i++; } b[i] = '\0';
    for (int j = i - 1; j >= 0; j--) { b[j] = (n % 10) + '0'; n /= 10; }
}

void GUI::fstr(float f, char* b) const
{
    int ip = (int)f; istr(ip, b);
    int l = clen(b); b[l++] = '.';
    float dp = f - ip;
    for (int i = 0; i < 2; i++) { dp *= 10; int d = (int)dp; b[l++] = d + '0'; dp -= d; }
    b[l] = '\0';
}

int GUI::toi(const char* s) const
{
    int r = 0; for (int i = 0; s[i]; i++) r = r * 10 + (s[i] - '0'); return r;
}

//Text Box
void TBox::init(sf::Font& f, float x, float y, float w, float h, bool pw)
{
    box.setPosition(x, y); box.setSize({ w,h });
    box.setOutlineThickness(2);
    isPass = pw; buf[0] = '\0'; len = 0;
    txt.setFont(f); txt.setCharacterSize(19);
    txt.setFillColor(sf::Color::Black);
    refresh();
}

void TBox::clear() { len = 0; buf[0] = '\0'; refresh(); }

void TBox::handle(sf::Event& e)
{
    if (!active || e.type != sf::Event::TextEntered) return;
    uint32_t c = e.text.unicode;
    if (c == 8) { if (len > 0) { buf[--len] = '\0'; refresh(); } }
    else if (c >= 32 && c < 127 && len < 510) { buf[len++] = (char)c; buf[len] = '\0'; refresh(); }
}

void TBox::refresh()
{
    char show[512];
    if (isPass) { for (int i = 0; i < len; i++) show[i] = '*'; show[len] = '\0'; }
    else { for (int i = 0; i <= len; i++) show[i] = buf[i]; }
    txt.setString(show);
    sf::Vector2f p = box.getPosition(), s = box.getSize();
    sf::FloatRect b = txt.getLocalBounds();
    txt.setPosition(p.x + 7.f, p.y + (s.y - b.height) / 2.f - 3.f);
    box.setFillColor(active ? sf::Color(240, 248, 255) : sf::Color(218, 218, 218));
    box.setOutlineColor(active ? sf::Color(50, 110, 180) : sf::Color(150, 150, 150));
}

//Constructor of GUI
GUI::GUI()
    : window(sf::VideoMode(820, 620), "MediCore Hospital Management System",
        sf::Style::Titlebar | sf::Style::Close),
    cur(Screen::MAIN_LOGIN), prev(Screen::MAIN_LOGIN),
    scrollY(0), logPat(nullptr), logDoc(nullptr),
    loginFails(0), bookDocID(-1)
{
    window.setFramerateLimit(60);
    if (!font.loadFromFile("arial.TTF"))
        font.loadFromFile("arial.ttf");   // fallback lowercase

    msgBuf[0] = listBuf[0] = errMsg[0] = '\0';
    bookSpec[0] = bookDate[0] = '\0';

    // initialise all text boxes (repositioned per-screen)
    for (int i = 0; i < 6; i++) tb[i].init(font, 0, 0, 300, 34);
}

//Handling Navigation
void GUI::goTo(Screen s)
{
    prev = cur; cur = s; scrollY = 0;
    errMsg[0] = '\0';
    for (int i = 0; i < 6; i++) { tb[i].clear(); tb[i].active = false; }
}

void GUI::showMsg(const char* m, Screen ret)
{
    cpy(msgBuf, m); prev = ret; cur = Screen::MESSAGE; scrollY = 0;
}

//some input helpers
bool GUI::wasClicked(sf::Event* e)
{
    return e && e->type == sf::Event::MouseButtonReleased && e->mouseButton.button == sf::Mouse::Left;
}

sf::Vector2f GUI::mousePos()
{
    return window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

void GUI::activateTB(int which, sf::Vector2f m, int count)
{
    for (int i = 0; i < count; i++) {
        tb[i].active = (i == which && tb[i].hit(m));
        tb[i].refresh();
    }
}

void GUI::handleTBs(sf::Event* e, int count)
{
    if (!e) return;
    if (e->type == sf::Event::MouseButtonPressed) {
        sf::Vector2f m = mousePos();
        for (int i = 0; i < count; i++) {
            tb[i].active = tb[i].hit(m);
            tb[i].refresh();
        }
    }
    for (int i = 0; i < count; i++) tb[i].handle(*e);
}

//drawing helpers
void GUI::drawHeaderBar(const char* subtitle)
{
    sf::RectangleShape hdr({ 820,54 });
    hdr.setFillColor(C_HDR); window.draw(hdr);
    drawLabel("MediCore Hospital Management System", 15, 10, C_WHITE, 22);
    if (subtitle) drawLabel(subtitle, 15, 34, sf::Color(180, 210, 255), 15);
}

void GUI::drawTitle(const char* t, float y, unsigned sz)
{
    sf::Text tx; tx.setFont(font); tx.setString(t);
    tx.setCharacterSize(sz); tx.setFillColor(C_HDR);
    tx.setStyle(sf::Text::Bold);
    sf::FloatRect b = tx.getLocalBounds();
    tx.setPosition((820 - b.width) / 2.f, y);
    window.draw(tx);
}

void GUI::drawLabel(const char* t, float x, float y, sf::Color c, unsigned sz)
{
    sf::Text tx; tx.setFont(font); tx.setString(t);
    tx.setCharacterSize(sz); tx.setFillColor(c);
    tx.setPosition(x, y); window.draw(tx);
}

void GUI::drawSep(float y)
{
    sf::RectangleShape l({ 780,2 }); l.setPosition(20, y);
    l.setFillColor(C_SEP); window.draw(l);
}

void GUI::drawScrollText(const char* text, float startY)
{
    float y = startY + scrollY;
    const char* p = text;
    char line[300]; int li = 0;
    while (*p)
    {
        if (*p == '\n' || li >= 298) {
            line[li] = '\0';
            if (y > 58 && y < 590) drawLabel(line, 25, y, C_TEXT, 16);
            y += 22; li = 0;
        }
        else { line[li++] = *p; }
        p++;
    }
    if (li > 0) { line[li] = '\0'; if (y > 58 && y < 590) drawLabel(line, 25, y, C_TEXT, 16); }
}

bool GUI::drawBtn(const char* lbl, float x, float y, float w, float h,
    sf::Event* e, sf::Color col, sf::Color hov)
{
    sf::Vector2f m = mousePos();
    sf::RectangleShape box({ w,h }); box.setPosition(x, y);
    bool over = box.getGlobalBounds().contains(m);
    box.setFillColor(over ? hov : col);
    window.draw(box);

    sf::Text t; t.setFont(font); t.setString(lbl);
    t.setCharacterSize(17); t.setFillColor(C_WHITE);
    sf::FloatRect b = t.getLocalBounds();
    t.setPosition(x + (w - b.width) / 2.f, y + (h - b.height) / 2.f - 3.f);
    window.draw(t);

    return over && wasClicked(e);
}

bool GUI::drawRow(int rowIdx, float y, float rowH, const char* text, sf::Event* e,
    bool hasBtn, const char* btnLbl, sf::Color btnCol)
{
    sf::RectangleShape row({ 780,rowH }); row.setPosition(20, y);
    row.setFillColor(rowIdx % 2 == 0 ? C_ROW_A : C_ROW_B);
    window.draw(row);
    drawLabel(text, 28, y + rowH / 2.f - 9.f, C_TEXT, 15);

    if (!hasBtn) return false;
    float bw = 80, bh = rowH - 10;
    float bx = 720, by = y + 5;
    return drawBtn(btnLbl, bx, by, bw, bh, e, btnCol, sf::Color(btnCol.r + 30, btnCol.g + 30, btnCol.b + 30));
}

//our main loop
void GUI::run()
{
    hospital.loadAll();

    while (window.isOpen())
    {
        sf::Event ev;
        bool has = window.pollEvent(ev);
        if (has && ev.type == sf::Event::Closed) window.close();

        // scroll
        if (has && ev.type == sf::Event::MouseWheelScrolled) {
            scrollY += ev.mouseWheelScroll.delta * 22.f;
            if (scrollY > 0) scrollY = 0;
        }

        window.clear(C_BG);
        sf::Event* ep = has ? &ev : nullptr;

        switch (cur)
        {
        case Screen::MAIN_LOGIN:sMainLogin(ep);    
            break;
        case Screen::PATIENT_LOGIN:sPatientLogin(ep); 
            break;
        case Screen::DOCTOR_LOGIN:sDoctorLogin(ep);  
            break;
        case Screen::ADMIN_LOGIN:sAdminLogin(ep);   
            break;
        case Screen::PATIENT_MENU:sPatientMenu(ep);  
            break;
        case Screen::DOCTOR_MENU:sDoctorMenu(ep);   
break;
        case Screen::ADMIN_MENU:sAdminMenu(ep);    
            break;
        case Screen::BOOK_SPEC:sBookSpec(ep);     
            break;
        case Screen::BOOK_DOCS:sBookDocs(ep);     
            break;
        case Screen::BOOK_DATE:sBookDate(ep);    
            break;
        case Screen::BOOK_SLOTS:sBookSlots(ep);    
            break;
        case Screen::CANCEL_APPT:sCancelAppt(ep);   
            break;
        case Screen::VIEW_MY_APPTS:sViewMyAppts(ep);  
            break;
        case Screen::VIEW_RECORDS:sViewRecords(ep);  
            break;
        case Screen::VIEW_BILLS:sViewBills(ep);
            break;
        case Screen::PAY_BILL:sPayBill(ep);      
            break;
        case Screen::TOPUP:sTopUp(ep);        
            break;
        case Screen::VIEW_TODAY:sViewToday(ep);    
            break;
        case Screen::MARK_COMPLETE:sMarkComplete(ep);
            break;
        case Screen::MARK_NOSHOW:sMarkNoShow(ep);   
            break;
        case Screen::WRITE_PRESC:sWritePresc(ep);   
            break;
        case Screen::VIEW_PAT_HIST:sViewPatHist(ep); 
            break;
        case Screen::ADD_DOCTOR:sAddDoctor(ep);    
            break;
        case Screen::REMOVE_DOCTOR:sRemoveDoctor(ep); 
            break;
        case Screen::ADD_PATIENT:sAddPatient(ep);   
            break;
        case Screen::REMOVE_PATIENT:sRemovePat(ep);    
            break;
        case Screen::VIEW_ALL_PATS:sViewAllPats(ep);  
            break;
        case Screen::VIEW_ALL_DOCS:sViewAllDocs(ep);  
            break;
        case Screen::VIEW_ALL_APPTS:sViewAllAppts(ep); 
            break;
        case Screen::VIEW_UNPAID:sViewUnpaid(ep);   
            break;
        case Screen::DISCHARGE:sDischarge(ep);    
            break;
        case Screen::SEC_LOG:sSecLog(ep);       
            break;
        case Screen::DAILY_REPORT:sDailyReport(ep);  
            break;
        case Screen::MESSAGE:sMessage(ep);      
            break;
        }

        window.display();
    }
}

//menu buttons
static int menuList(sf::RenderWindow& win, sf::Font& font, const char** labels, int n, float startY, sf::Vector2f m, bool clicked, sf::Color col = sf::Color(50, 110, 180))
{
    for (int i = 0; i < n; i++) 
    {
        float y = startY + i * 50.f;
        sf::RectangleShape b({ 420,40 });
        b.setPosition(200, y);
        bool ov = b.getGlobalBounds().contains(m);
        b.setFillColor(ov ? sf::Color(80, 150, 220) : col);
        win.draw(b);
        sf::Text t; t.setFont(font); t.setString(labels[i]);
        t.setCharacterSize(18); t.setFillColor(sf::Color::White);
        sf::FloatRect lb = t.getLocalBounds();
        t.setPosition(200 + (420 - lb.width) / 2.f, y + (40 - lb.height) / 2.f - 3.f);
        win.draw(t);
        if (ov && clicked) return i;
    }
    return -1;
}

//main login page
void GUI::sMainLogin(sf::Event* e)
{
    drawHeaderBar();
    drawTitle("Welcome to MediCore", 72);
    drawSep(112);
    drawLabel("Select your role to continue:", 200, 126, C_TEXT, 18);

    sf::Vector2f m = mousePos();
    bool cl = wasClicked(e);

    const char* lbl[] = { "Patient Login","Doctor Login","Admin Login","Exit" };
    int idx = menuList(window, font, lbl, 4, 150, m, cl);
    if (idx == 0) { loginFails = 0; goTo(Screen::PATIENT_LOGIN); }
    if (idx == 1) { loginFails = 0; goTo(Screen::DOCTOR_LOGIN); }
    if (idx == 2) { loginFails = 0; goTo(Screen::ADMIN_LOGIN); }
    if (idx == 3) window.close();
}

static void loginLayout(GUI* gui, sf::RenderWindow& win, sf::Font& font, TBox* tbID, TBox* tbPass,
    const char* title, const char* idLabel)
{
    tbID->box.setPosition(260, 230); tbID->box.setSize({ 300,34 });
    tbPass->box.setPosition(260, 298); tbPass->box.setSize({ 300,34 });
    tbID->refresh(); tbPass->refresh();
}

// Patient Login
void GUI::sPatientLogin(sf::Event* e)
{
    tb[0].isPass = false; tb[1].isPass = true;
    tb[0].box.setPosition(260, 230); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    tb[1].box.setPosition(260, 298); tb[1].box.setSize({ 300,34 }); tb[1].refresh();
    handleTBs(e, 2);

    drawHeaderBar("Patient Login");
    drawTitle("Patient Login", 70);
    drawSep(110);
    drawLabel("Patient ID:", 160, 234, C_TEXT);
    tb[0].draw(window);
    drawLabel("Password:", 160, 302, C_TEXT);
    tb[1].draw(window);
    if (errMsg[0]) drawLabel(errMsg, 160, 344, C_RED, 16);

    if (drawBtn("Login", 260, 370, 140, 36, e)) {
        Patient* p = hospital.guiLoginPatient(tb[0].buf, tb[1].buf);
        if (p) { logPat = p; loginFails = 0; errMsg[0] = '\0'; goTo(Screen::PATIENT_MENU); }
        else {
            loginFails++;
            hospital.logSec("Patient", tb[0].buf, "FAILED");
            if (loginFails >= 3) showMsg("Account locked. Contact admin.", Screen::MAIN_LOGIN);
            else cpy(errMsg, "Invalid ID or password.");
        }
    }
    if (drawBtn("Back", 420, 370, 140, 36, e, C_GRAY)) goTo(Screen::MAIN_LOGIN);
}

// Doctor Login
void GUI::sDoctorLogin(sf::Event* e)
{
    tb[0].isPass = false; tb[1].isPass = true;
    tb[0].box.setPosition(260, 230); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    tb[1].box.setPosition(260, 298); tb[1].box.setSize({ 300,34 }); tb[1].refresh();
    handleTBs(e, 2);

    drawHeaderBar("Doctor Login");
    drawTitle("Doctor Login", 70);
    drawSep(110);
    drawLabel("Doctor ID:", 160, 234, C_TEXT);
    tb[0].draw(window);
    drawLabel("Password:", 160, 302, C_TEXT);
    tb[1].draw(window);
    if (errMsg[0]) drawLabel(errMsg, 160, 344, C_RED, 16);

    if (drawBtn("Login", 260, 370, 140, 36, e)) {
        Doctor* d = hospital.guiLoginDoctor(tb[0].buf, tb[1].buf);
        if (d) { logDoc = d; loginFails = 0; errMsg[0] = '\0'; goTo(Screen::DOCTOR_MENU); }
        else {
            loginFails++;
            hospital.logSec("Doctor", tb[0].buf, "FAILED");
            if (loginFails >= 3) showMsg("Account locked. Contact admin.", Screen::MAIN_LOGIN);
            else cpy(errMsg, "Invalid ID or password.");
        }
    }
    if (drawBtn("Back", 420, 370, 140, 36, e, C_GRAY)) goTo(Screen::MAIN_LOGIN);
}

// Admin Login
void GUI::sAdminLogin(sf::Event* e)
{
    tb[0].isPass = false; tb[1].isPass = true;
    tb[0].box.setPosition(260, 230); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    tb[1].box.setPosition(260, 298); tb[1].box.setSize({ 300,34 }); tb[1].refresh();
    handleTBs(e, 2);

    drawHeaderBar("Admin Login");
    drawTitle("Admin Login", 70);
    drawSep(110);
    drawLabel("Admin ID:", 160, 234, C_TEXT);
    tb[0].draw(window);
    drawLabel("Password:", 160, 302, C_TEXT);
    tb[1].draw(window);
    if (errMsg[0]) drawLabel(errMsg, 160, 344, C_RED, 16);

    if (drawBtn("Login", 260, 370, 140, 36, e)) {
        if (hospital.guiLoginAdmin(tb[0].buf, tb[1].buf)) {
            loginFails = 0; errMsg[0] = '\0'; goTo(Screen::ADMIN_MENU);
        }
        else {
            loginFails++;
            hospital.logSec("Admin", tb[0].buf, "FAILED");
            if (loginFails >= 3) showMsg("Account locked. Contact admin.", Screen::MAIN_LOGIN);
            else cpy(errMsg, "Invalid ID or password.");
        }
    }
    if (drawBtn("Back", 420, 370, 140, 36, e, C_GRAY)) goTo(Screen::MAIN_LOGIN);
}

//patient's menu
void GUI::sPatientMenu(sf::Event* e)
{
    if (!logPat) { goTo(Screen::MAIN_LOGIN); return; }

    char sub[120]; cpy(sub, "Logged in: "); cat(sub, logPat->getName());
    cat(sub, "   Balance: PKR ");
    char bb[20]; fstr(logPat->getBalance(), bb); cat(sub, bb);
    drawHeaderBar(sub);
    drawTitle("Patient Menu", 70);
    drawSep(110);

    sf::Vector2f m = mousePos(); bool cl = wasClicked(e);
    const char* lbl[] = { "1. Book Appointment","2. Cancel Appointment", "3. View My Appointments","4. Medical Records", "5. View Bills","6. Pay Bill", "7. Top Up Balance","8. Logout" };
    int idx = menuList(window, font, lbl, 8, 120, m, cl);
    switch (idx) {
    case 0: goTo(Screen::BOOK_SPEC);     break;
    case 1: goTo(Screen::CANCEL_APPT);   break;
    case 2: goTo(Screen::VIEW_MY_APPTS); break;
    case 3: goTo(Screen::VIEW_RECORDS);  break;
    case 4: goTo(Screen::VIEW_BILLS);    break;
    case 5: goTo(Screen::PAY_BILL);      break;
    case 6: goTo(Screen::TOPUP);         break;
    case 7: logPat = nullptr; goTo(Screen::MAIN_LOGIN); break;
    }
}

//doctor's menu
void GUI::sDoctorMenu(sf::Event* e)
{
    if (!logDoc) { goTo(Screen::MAIN_LOGIN); return; }

    char sub[120]; cpy(sub, "Dr. "); cat(sub, logDoc->getName());
    cat(sub, "  |  "); cat(sub, logDoc->getSpecialization());
    drawHeaderBar(sub);
    drawTitle("Doctor Menu", 70);
    drawSep(110);

    sf::Vector2f m = mousePos(); bool cl = wasClicked(e);
    const char* lbl[] = { "1. View Today's Appointments","2. Mark Appointment Complete", "3. Mark Appointment No-Show","4. Write Prescription", "5. View Patient Medical History","6. Logout" };
    int idx = menuList(window, font, lbl, 6, 130, m, cl);
    switch (idx) {
    case 0: goTo(Screen::VIEW_TODAY);    break;
    case 1: goTo(Screen::MARK_COMPLETE); break;
    case 2: goTo(Screen::MARK_NOSHOW);   break;
    case 3: goTo(Screen::WRITE_PRESC);   break;
    case 4: goTo(Screen::VIEW_PAT_HIST); break;
    case 5: logDoc = nullptr; goTo(Screen::MAIN_LOGIN); break;
    }
}

//admin's menu (this has a 2 column layout)
void GUI::sAdminMenu(sf::Event* e)
{
    drawHeaderBar("Admin Panel");
    drawTitle("Admin Panel", 70);
    drawSep(110);

    sf::Vector2f m = mousePos(); bool cl = wasClicked(e);
    struct Item { const char* lbl; Screen scr; };
    Item items[] = {
        {"1. Add Doctor",        Screen::ADD_DOCTOR},
        {"2. Remove Doctor",     Screen::REMOVE_DOCTOR},
        {"3. Add Patient",       Screen::ADD_PATIENT},
        {"4. Remove Patient",    Screen::REMOVE_PATIENT},
        {"5. View All Patients", Screen::VIEW_ALL_PATS},
        {"6. View All Doctors",  Screen::VIEW_ALL_DOCS},
        {"7. View All Appts",    Screen::VIEW_ALL_APPTS},
        {"8. View Unpaid Bills", Screen::VIEW_UNPAID},
        {"9. Discharge Patient", Screen::DISCHARGE},
        {"10. Security Log",     Screen::SEC_LOG},
        {"11. Daily Report",     Screen::DAILY_REPORT},
        {"12. Logout",           Screen::MAIN_LOGIN}
    };
    int n = 12;
    for (int i = 0; i < n; i++) {
        int col = i % 2, row = i / 2;
        float x = col == 0 ? 20.f : 420.f;
        float y = 120.f + row * 62.f;
        float w = 380, h = 52;
        sf::RectangleShape box({ w,h }); box.setPosition(x, y);
        bool ov = box.getGlobalBounds().contains(m);
        box.setFillColor(ov ? C_HOV : C_BTN); window.draw(box);
        sf::Text t; t.setFont(font); t.setString(items[i].lbl);
        t.setCharacterSize(17); t.setFillColor(C_WHITE);
        sf::FloatRect lb = t.getLocalBounds();
        t.setPosition(x + (w - lb.width) / 2.f, y + (h - lb.height) / 2.f - 3.f);
        window.draw(t);
        if (ov && cl) {
            if (items[i].scr == Screen::MAIN_LOGIN) logPat = nullptr;
            goTo(items[i].scr);
            return;
        }
    }
}

//specialization
void GUI::sBookSpec(sf::Event* e)
{
    tb[0].isPass = false;
    tb[0].box.setPosition(160, 230); tb[0].box.setSize({ 500,34 }); tb[0].refresh();
    handleTBs(e, 1);

    drawHeaderBar("Book Appointment"); drawTitle("Book Appointment", 70); drawSep(110);
    drawLabel("Enter specialization (e.g. Cardiology):", 160, 200, C_TEXT);
    tb[0].draw(window);

    if (drawBtn("Search", 280, 290, 130, 36, e)) {
        cpy(bookSpec, tb[0].buf);
        if (clen(bookSpec) > 0) goTo(Screen::BOOK_DOCS);
    }
    if (drawBtn("Back", 430, 290, 130, 36, e, C_GRAY)) goTo(Screen::PATIENT_MENU);
}

//choose doctor
void GUI::sBookDocs(sf::Event* e)
{
    drawHeaderBar("Book Appointment"); drawTitle("Select Doctor", 70); drawSep(110);

    // Column headers
    drawLabel("ID   | Name                | Specialization       | Fee (PKR)", 25, 118, C_HDR, 15);
    drawSep(136);

    float y = 140 + scrollY;
    bool any = false;
    int rowIdx = 0;
    for (int i = 0; i < hospital.getDoctors().size(); i++) {
        Doctor* d = hospital.getDoctors().getDataAtIndex(i);
        if (!d) continue;
        if (!ceqCI(d->getSpecialization(), bookSpec)) continue;
        any = true;
        if (y < 138 || y>570) { y += 44; rowIdx++; continue; }

        char line[200], idBuf[10], feeBuf[20];
        istr(d->getID(), idBuf); fstr(d->getFee(), feeBuf);
        cpy(line, idBuf); cat(line, "   | "); cat(line, d->getName());
        cat(line, "  | "); cat(line, d->getSpecialization());
        cat(line, "  | "); cat(line, feeBuf);

        if (drawRow(rowIdx, y, 40, line, e, true, "Select", C_GREEN)) {
            bookDocID = d->getID(); goTo(Screen::BOOK_DATE); return;
        }
        y += 44; rowIdx++;
    }
    if (!any) drawLabel("No doctors found for that specialization.", 160, 220, C_RED);

    if (drawBtn("Back", 330, 575, 160, 34, e, C_GRAY)) goTo(Screen::BOOK_SPEC);
}

//date
void GUI::sBookDate(sf::Event* e)
{
    tb[0].isPass = false;
    tb[0].box.setPosition(260, 230); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    handleTBs(e, 1);

    drawHeaderBar("Book Appointment"); drawTitle("Enter Date", 70); drawSep(110);
    drawLabel("Date (DD-MM-YYYY):", 160, 202, C_TEXT);
    tb[0].draw(window);
    if (errMsg[0]) drawLabel(errMsg, 160, 274, C_RED, 16);

    if (drawBtn("Next", 260, 300, 130, 36, e)) {
        Validator v;
        if (!v.check_date(tb[0].buf)) { cpy(errMsg, "Invalid date. Use DD-MM-YYYY."); }
        else { cpy(bookDate, tb[0].buf); errMsg[0] = '\0'; goTo(Screen::BOOK_SLOTS); }
    }
    if (drawBtn("Back", 410, 300, 130, 36, e, C_GRAY)) goTo(Screen::BOOK_DOCS);
}

//slot
void GUI::sBookSlots(sf::Event* e)
{
    drawHeaderBar("Book Appointment"); drawTitle("Select Time Slot", 70); drawSep(110);

    char hdr[100]; cpy(hdr, "Doctor ID: ");
    char db[10]; istr(bookDocID, db); cat(hdr, db);
    cat(hdr, "   Date: "); cat(hdr, bookDate);
    drawLabel(hdr, 20, 118, C_HDR, 15);
    if (errMsg[0]) drawLabel(errMsg, 20, 140, C_RED, 15);

    const char* slots[8] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };
    float y = 162;
    for (int s = 0; s < 8; s++) {
        bool taken = false;
        for (int i = 0; i < hospital.getAppointments().size(); i++) {
            Appointment* a = hospital.getAppointments().getDataAtIndex(i);
            if (!a) continue;
            if (a->getDoctorID() == bookDocID &&
                hospital.strEq(a->getDate(), bookDate) &&
                hospital.strEq(a->getTimeSlot(), slots[s]) &&
                !hospital.strEq(a->getStatus(), "cancelled"))
            {
                taken = true; break;
            }
        }

        sf::RectangleShape row({ 780,36 }); row.setPosition(20, y);
        row.setFillColor(taken ? sf::Color(230, 210, 210) : C_ROW_B);
        window.draw(row);

        drawLabel(slots[s], 35, y + 8, taken ? sf::Color(160, 80, 80) : C_TEXT, 18);
        drawLabel(taken ? "Taken" : "Available", 200, y + 8,
            taken ? sf::Color(160, 80, 80) : C_GREEN, 16);

        if (!taken) {
            if (drawBtn("Book", 700, y + 4, 72, 28, e, C_BTN)) {
                char result[300];
                hospital.guiBookAppointment(logPat, bookDocID, bookDate, slots[s], result);
                showMsg(result, Screen::PATIENT_MENU);
                return;
            }
        }
        y += 40;
    }
    if (drawBtn("Back", 330, 575, 160, 34, e, C_GRAY)) goTo(Screen::BOOK_DATE);
}

//cancel an appointment
void GUI::sCancelAppt(sf::Event* e)
{
    drawHeaderBar("Cancel Appointment"); drawTitle("Cancel Appointment", 70); drawSep(110);
    drawLabel("ID   | Doctor              | Date       | Slot", 25, 118, C_HDR, 15);
    drawSep(136);

    float y = 140 + scrollY; bool any = false; int ri = 0;
    for (int i = 0; i < hospital.getAppointments().size(); i++) {
        Appointment* a = hospital.getAppointments().getDataAtIndex(i);
        if (!a || a->getPatientID() != logPat->getID()) continue;
        if (!hospital.strEq(a->getStatus(), "pending")) continue;
        any = true;
        if (y < 138 || y>570) { y += 42; ri++; continue; }

        Doctor* d = hospital.getDoctors().findByID(a->getDoctorID());
        char line[200], idBuf[10];
        istr(a->getApptID(), idBuf);
        cpy(line, idBuf); cat(line, "   | ");
        cat(line, d ? d->getName() : "N/A");
        cat(line, "  | "); cat(line, a->getDate());
        cat(line, "  "); cat(line, a->getTimeSlot());

        if (drawRow(ri, y, 40, line, e, true, "Cancel", C_RED)) {
            char result[300];
            hospital.guiCancelAppointment(logPat, a->getApptID(), result);
            showMsg(result, Screen::PATIENT_MENU); return;
        }
        y += 42; ri++;
    }
    if (!any) drawLabel("No pending appointments.", 160, 230, C_TEXT);
    if (drawBtn("Back", 330, 575, 160, 34, e, C_GRAY)) goTo(Screen::PATIENT_MENU);
}

//view my appointments
void GUI::sViewMyAppts(sf::Event* e)
{
    drawHeaderBar("My Appointments"); drawTitle("My Appointments", 70); drawSep(110);
    drawLabel("ID  | Doctor          | Date       | Slot  | Status", 25, 118, C_HDR, 15);
    drawSep(136);

    float y = 140 + scrollY; bool any = false; int ri = 0;
    for (int i = 0; i < hospital.getAppointments().size(); i++) {
        Appointment* a = hospital.getAppointments().getDataAtIndex(i);
        if (!a || a->getPatientID() != logPat->getID()) continue;
        any = true;
        if (y < 138 || y>570) { y += 34; ri++; continue; }

        Doctor* d = hospital.getDoctors().findByID(a->getDoctorID());
        char line[200], idBuf[10];
        istr(a->getApptID(), idBuf);
        cpy(line, idBuf); cat(line, "   | ");
        cat(line, d ? d->getName() : "N/A");
        cat(line, "  | "); cat(line, a->getDate());
        cat(line, "  | "); cat(line, a->getTimeSlot());
        cat(line, "  | "); cat(line, a->getStatus());
        drawRow(ri, y, 32, line, e);
        y += 34; ri++;
    }
    if (!any) drawLabel("No appointments found.", 160, 230, C_TEXT);
    if (drawBtn("Back", 330, 580, 160, 34, e, C_GRAY)) goTo(Screen::PATIENT_MENU);
}

//view my medical records
void GUI::sViewRecords(sf::Event* e)
{
    drawHeaderBar("Medical Records"); drawTitle("Medical Records", 70); drawSep(110);

    float y = 120 + scrollY; bool any = false; int ri = 0;
    for (int i = 0; i < hospital.getPrescriptions().size(); i++) {
        Prescription* pr = hospital.getPrescriptions().getDataAtIndex(i);
        if (!pr || pr->getPatientID() != logPat->getID()) continue;
        any = true;
        if (y < 118 || y>570) { y += 80; ri++; continue; }

        Doctor* d = hospital.getDoctors().findByID(pr->getDoctorID());
        sf::RectangleShape card({ 780,74 }); card.setPosition(20, y);
        card.setFillColor(ri % 2 == 0 ? C_ROW_A : C_ROW_B); window.draw(card);

        char l1[120], l2[300], l3[300];
        cpy(l1, "Date: "); cat(l1, pr->getDate()); cat(l1, "  |  Dr. ");
        cat(l1, d ? d->getName() : "N/A");
        cpy(l2, "Medicines: "); cat(l2, pr->getMedicines());
        cpy(l3, "Notes: ");     cat(l3, pr->getNotes());
        drawLabel(l1, 30, y + 4, C_HDR, 15);
        drawLabel(l2, 30, y + 26, C_TEXT, 14);
        drawLabel(l3, 30, y + 48, C_TEXT, 14);
        y += 80; ri++;
    }
    if (!any) drawLabel("No medical records found.", 160, 230, C_TEXT);
    if (drawBtn("Back", 330, 580, 160, 34, e, C_GRAY)) goTo(Screen::PATIENT_MENU);
}

//view my bill
void GUI::sViewBills(sf::Event* e)
{
    drawHeaderBar("My Bills"); drawTitle("My Bills", 70); drawSep(110);
    drawLabel("Bill | Appt | Amount (PKR) | Status  | Date", 25, 118, C_HDR, 15);
    drawSep(136);

    float y = 140 + scrollY; bool any = false; int ri = 0; float tot = 0;
    for (int i = 0; i < hospital.getBills().size(); i++) {
        Bill* b = hospital.getBills().getDataAtIndex(i);
        if (!b || b->getPatientID() != logPat->getID()) continue;
        any = true;
        if (hospital.strEq(b->getStatus(), "unpaid")) tot += b->getAmount();
        if (y < 138 || y>555) { y += 32; ri++; continue; }

        char line[200], bBuf[10], aBuf[10], amtBuf[20];
        istr(b->getBillID(), bBuf); istr(b->getAppointmentID(), aBuf);
        fstr(b->getAmount(), amtBuf);
        cpy(line, bBuf); cat(line, "   | "); cat(line, aBuf);
        cat(line, "    | "); cat(line, amtBuf);
        cat(line, "     | "); cat(line, b->getStatus());
        cat(line, "  | "); cat(line, b->getDate());

        sf::Color rc = hospital.strEq(b->getStatus(), "unpaid") ? C_RED :
            hospital.strEq(b->getStatus(), "paid") ? C_GREEN : C_TEXT;

        sf::RectangleShape row({ 780,30 }); row.setPosition(20, y);
        row.setFillColor(ri % 2 == 0 ? C_ROW_A : C_ROW_B); window.draw(row);
        drawLabel(line, 28, y + 6, rc, 14);
        y += 32; ri++;
    }
    if (!any) drawLabel("No bills found.", 160, 230, C_TEXT);
    else {
        char t2[60]; cpy(t2, "Total Outstanding: PKR "); char tb2[20]; fstr(tot, tb2); cat(t2, tb2);
        drawLabel(t2, 25, 560, C_RED, 16);
    }
    if (drawBtn("Back", 330, 580, 160, 28, e, C_GRAY)) goTo(Screen::PATIENT_MENU);
}

//pay the bill
void GUI::sPayBill(sf::Event* e)
{
    drawHeaderBar("Pay Bill"); drawTitle("Pay Bill", 70); drawSep(110);
    drawLabel("Bill ID | Amount (PKR) | Date", 25, 118, C_HDR, 15);
    drawSep(136);

    float y = 140 + scrollY; bool any = false; int ri = 0;
    for (int i = 0; i < hospital.getBills().size(); i++) {
        Bill* b = hospital.getBills().getDataAtIndex(i);
        if (!b || b->getPatientID() != logPat->getID()) continue;
        if (!hospital.strEq(b->getStatus(), "unpaid")) continue;
        any = true;
        if (y < 138 || y>555) { y += 42; ri++; continue; }

        char line[200], bBuf[10], amtBuf[20];
        istr(b->getBillID(), bBuf); fstr(b->getAmount(), amtBuf);
        cpy(line, "Bill "); cat(line, bBuf); cat(line, "  |  PKR ");
        cat(line, amtBuf); cat(line, "  |  "); cat(line, b->getDate());

        if (drawRow(ri, y, 40, line, e, true, "Pay", C_GREEN)) {
            char result[300];
            hospital.guiPayBill(logPat, b->getBillID(), result);
            showMsg(result, Screen::PATIENT_MENU); return;
        }
        y += 42; ri++;
    }
    if (!any) drawLabel("No unpaid bills.", 160, 230, C_TEXT);
    if (drawBtn("Back", 330, 575, 160, 34, e, C_GRAY)) goTo(Screen::PATIENT_MENU);
}

//top up
void GUI::sTopUp(sf::Event* e)
{
    tb[0].isPass = false;
    tb[0].box.setPosition(260, 240); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    handleTBs(e, 1);

    drawHeaderBar("Top Up Balance"); drawTitle("Top Up Balance", 70); drawSep(110);
    char balLine[80]; cpy(balLine, "Current Balance: PKR ");
    char bb[20]; fstr(logPat->getBalance(), bb); cat(balLine, bb);
    drawLabel(balLine, 160, 150, C_TEXT);
    drawLabel("Amount to add (PKR):", 160, 212, C_TEXT);
    tb[0].draw(window);
    if (errMsg[0]) drawLabel(errMsg, 160, 284, C_RED, 16);

    if (drawBtn("Add", 260, 308, 130, 36, e)) {
        char result[300];
        hospital.guiTopUp(logPat, tb[0].buf, result);
        showMsg(result, Screen::PATIENT_MENU);
    }
    if (drawBtn("Back", 410, 308, 130, 36, e, C_GRAY)) goTo(Screen::PATIENT_MENU);
}

//doctor portion
//view today
void GUI::sViewToday(sf::Event* e)
{
    drawHeaderBar("Today's Appointments"); drawTitle("Today's Appointments", 70); drawSep(110);
    drawLabel("ID   | Patient            | Slot  | Status", 25, 118, C_HDR, 15);
    drawSep(136);

    char today[11]; hospital.getTodayStr(today);
    float y = 140 + scrollY; bool any = false; int ri = 0;
    for (int i = 0; i < hospital.getAppointments().size(); i++) {
        Appointment* a = hospital.getAppointments().getDataAtIndex(i);
        if (!a || a->getDoctorID() != logDoc->getID()) continue;
        if (!hospital.strEq(a->getDate(), today)) continue;
        any = true;
        if (y < 138 || y>570) { y += 34; ri++; continue; }

        Patient* pt = hospital.getPatients().findByID(a->getPatientID());
        char line[200], idBuf[10]; istr(a->getApptID(), idBuf);
        cpy(line, idBuf); cat(line, "   | ");
        cat(line, pt ? pt->getName() : "N/A");
        cat(line, "  | "); cat(line, a->getTimeSlot());
        cat(line, "  | "); cat(line, a->getStatus());
        drawRow(ri, y, 32, line, e);
        y += 34; ri++;
    }
    if (!any) drawLabel("No appointments today.", 160, 230, C_TEXT);
    if (drawBtn("Back", 330, 580, 160, 34, e, C_GRAY)) goTo(Screen::DOCTOR_MENU);
}

//mark complete/no show
void GUI::sMarkComplete(sf::Event* e)
{
    tb[0].isPass = false;
    tb[0].box.setPosition(260, 240); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    handleTBs(e, 1);

    drawHeaderBar("Mark Complete"); drawTitle("Mark Appointment Complete", 70); drawSep(110);
    drawLabel("Enter Appointment ID:", 160, 212, C_TEXT);
    tb[0].draw(window);

    if (drawBtn("Mark Complete", 240, 300, 180, 36, e, C_GREEN)) {
        char result[300];
        hospital.guiMarkComplete(logDoc, tb[0].buf, result);
        showMsg(result, Screen::DOCTOR_MENU);
    }
    if (drawBtn("Back", 440, 300, 130, 36, e, C_GRAY)) goTo(Screen::DOCTOR_MENU);
}

void GUI::sMarkNoShow(sf::Event* e)
{
    tb[0].isPass = false;
    tb[0].box.setPosition(260, 240); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    handleTBs(e, 1);

    drawHeaderBar("Mark No-Show"); drawTitle("Mark Appointment No-Show", 70); drawSep(110);
    drawLabel("Enter Appointment ID:", 160, 212, C_TEXT);
    tb[0].draw(window);

    if (drawBtn("Mark No-Show", 240, 300, 180, 36, e, C_RED)) {
        char result[300];
        hospital.guiMarkNoShow(logDoc, tb[0].buf, result);
        showMsg(result, Screen::DOCTOR_MENU);
    }
    if (drawBtn("Back", 440, 300, 130, 36, e, C_GRAY)) goTo(Screen::DOCTOR_MENU);
}

//write prescription
void GUI::sWritePresc(sf::Event* e)
{
    // tb0 = ApptID  tb1 = medicines  tb2 = notes
    tb[0].isPass = false; tb[0].box.setPosition(260, 150); tb[0].box.setSize({ 250,32 }); tb[0].refresh();
    tb[1].isPass = false; tb[1].box.setPosition(160, 248); tb[1].box.setSize({ 520,32 }); tb[1].refresh();
    tb[2].isPass = false; tb[2].box.setPosition(160, 320); tb[2].box.setSize({ 520,32 }); tb[2].refresh();
    handleTBs(e, 3);

    drawHeaderBar("Write Prescription"); drawTitle("Write Prescription", 70); drawSep(110);
    drawLabel("Appointment ID:", 160, 122, C_TEXT);
    tb[0].draw(window);
    drawLabel("Medicines (e.g. Paracetamol 500mg; Ibuprofen 200mg):", 160, 222, C_TEXT, 15);
    tb[1].draw(window);
    drawLabel("Notes:", 160, 294, C_TEXT, 15);
    tb[2].draw(window);
    if (errMsg[0]) drawLabel(errMsg, 160, 362, C_RED, 15);

    if (drawBtn("Save", 220, 378, 150, 36, e, C_BTN)) {
        char result[300];
        hospital.guiWritePrescription(logDoc, tb[0].buf, tb[1].buf, tb[2].buf, result);
        showMsg(result, Screen::DOCTOR_MENU);
    }
    if (drawBtn("Back", 400, 378, 130, 36, e, C_GRAY)) goTo(Screen::DOCTOR_MENU);
}

//view the patient's history
void GUI::sViewPatHist(sf::Event* e)
{
    tb[0].isPass = false;
    tb[0].box.setPosition(260, 230); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    handleTBs(e, 1);

    drawHeaderBar("Patient History"); drawTitle("Patient Medical History", 70); drawSep(110);
    drawLabel("Enter Patient ID:", 160, 202, C_TEXT);
    tb[0].draw(window);

    if (drawBtn("View", 260, 290, 130, 36, e)) {
        char result[2000];
        hospital.guiViewPatientHistory(logDoc, tb[0].buf, result);
        showMsg(result, Screen::DOCTOR_MENU);
    }
    if (drawBtn("Back", 410, 290, 130, 36, e, C_GRAY)) goTo(Screen::DOCTOR_MENU);
}

//admin portion
//add doctor
void GUI::sAddDoctor(sf::Event* e)
{
    // tb0 = name tb1 =spec tb2= contact tb3 =pass tb4 =fee
    float bx = 300; float bw = 380;
    tb[0].isPass = false; tb[0].box.setPosition(bx, 115); tb[0].box.setSize({ bw,30 }); tb[0].refresh();
    tb[1].isPass = false; tb[1].box.setPosition(bx, 157); tb[1].box.setSize({ bw,30 }); tb[1].refresh();
    tb[2].isPass = false; tb[2].box.setPosition(bx, 199); tb[2].box.setSize({ bw,30 }); tb[2].refresh();
    tb[3].isPass = true;  tb[3].box.setPosition(bx, 241); tb[3].box.setSize({ bw,30 }); tb[3].refresh();
    tb[4].isPass = false; tb[4].box.setPosition(bx, 283); tb[4].box.setSize({ bw,30 }); tb[4].refresh();
    handleTBs(e, 5);

    drawHeaderBar("Add Doctor"); drawTitle("Add Doctor", 70); drawSep(108);
    drawLabel("Name:", 160, 118, C_TEXT, 16); tb[0].draw(window);
    drawLabel("Specialization:", 160, 160, C_TEXT, 16); tb[1].draw(window);
    drawLabel("Contact (11 dig):", 160, 202, C_TEXT, 16); tb[2].draw(window);
    drawLabel("Password (min 6):", 160, 244, C_TEXT, 16); tb[3].draw(window);
    drawLabel("Fee (PKR):", 160, 286, C_TEXT, 16); tb[4].draw(window);
    if (errMsg[0]) drawLabel(errMsg, 160, 326, C_RED, 15);

    if (drawBtn("Add Doctor", 200, 352, 160, 36, e, C_GREEN)) {
        char result[300];
        hospital.guiAddDoctor(tb[0].buf, tb[1].buf, tb[2].buf, tb[3].buf, tb[4].buf, result);
        showMsg(result, Screen::ADMIN_MENU);
    }
    if (drawBtn("Back", 390, 352, 130, 36, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//remove a doctor
void GUI::sRemoveDoctor(sf::Event* e)
{
    drawHeaderBar("Remove Doctor"); drawTitle("Remove Doctor", 70); drawSep(110);
    drawLabel("ID   | Name              | Specialization    | Fee", 25, 118, C_HDR, 15);
    drawSep(136);

    float y = 140 + scrollY; int ri = 0;
    for (int i = 0; i < hospital.getDoctors().size(); i++) {
        Doctor* d = hospital.getDoctors().getDataAtIndex(i);
        if (!d) continue;
        if (y < 138 || y>570) { y += 36; ri++; continue; }

        char line[200], idBuf[10], feeBuf[20];
        istr(d->getID(), idBuf); fstr(d->getFee(), feeBuf);
        cpy(line, idBuf); cat(line, "   | "); cat(line, d->getName());
        cat(line, "  | "); cat(line, d->getSpecialization());
        cat(line, "  | "); cat(line, feeBuf);

        if (drawRow(ri, y, 34, line, e, true, "Remove", C_RED)) {
            char result[300];
            hospital.guiRemoveDoctor(d->getID(), result);
            showMsg(result, Screen::ADMIN_MENU); return;
        }
        y += 36; ri++;
    }
    if (drawBtn("Back", 330, 580, 160, 34, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//add patient
void GUI::sAddPatient(sf::Event* e)
{
    float bx = 300, bw = 380;
    tb[0].isPass = false; tb[0].box.setPosition(bx, 108); tb[0].box.setSize({ bw,28 }); tb[0].refresh();
    tb[1].isPass = true;  tb[1].box.setPosition(bx, 146); tb[1].box.setSize({ bw,28 }); tb[1].refresh();
    tb[2].isPass = false; tb[2].box.setPosition(bx, 184); tb[2].box.setSize({ bw,28 }); tb[2].refresh();
    tb[3].isPass = false; tb[3].box.setPosition(bx, 222); tb[3].box.setSize({ bw,28 }); tb[3].refresh();
    tb[4].isPass = false; tb[4].box.setPosition(bx, 260); tb[4].box.setSize({ bw,28 }); tb[4].refresh();
    tb[5].isPass = false; tb[5].box.setPosition(bx, 298); tb[5].box.setSize({ bw,28 }); tb[5].refresh();
    handleTBs(e, 6);

    drawHeaderBar("Add Patient"); drawTitle("Add Patient", 70); drawSep(106);
    drawLabel("Name:", 160, 111, C_TEXT, 15); tb[0].draw(window);
    drawLabel("Password (min6):", 160, 149, C_TEXT, 15); tb[1].draw(window);
    drawLabel("Age:", 160, 187, C_TEXT, 15); tb[2].draw(window);
    drawLabel("Gender (M/F):", 160, 225, C_TEXT, 15); tb[3].draw(window);
    drawLabel("Contact (11dig):", 160, 263, C_TEXT, 15); tb[4].draw(window);
    drawLabel("Balance (PKR):", 160, 301, C_TEXT, 15); tb[5].draw(window);
    if (errMsg[0]) drawLabel(errMsg, 160, 338, C_RED, 14);

    if (drawBtn("Add Patient", 200, 362, 160, 34, e, C_GREEN)) {
        char result[300];
        hospital.guiAddPatient(tb[0].buf, tb[1].buf, tb[2].buf,
            tb[3].buf, tb[4].buf, tb[5].buf, result);
        showMsg(result, Screen::ADMIN_MENU);
    }
    if (drawBtn("Back", 390, 362, 130, 34, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//remove patient
void GUI::sRemovePat(sf::Event* e)
{
    tb[0].isPass = false;
    tb[0].box.setPosition(260, 230); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    handleTBs(e, 1);

    drawHeaderBar("Remove Patient"); drawTitle("Remove Patient", 70); drawSep(110);
    drawLabel("Enter Patient ID:", 160, 202, C_TEXT);
    tb[0].draw(window);

    if (drawBtn("Remove", 260, 290, 130, 36, e, C_RED)) {
        char result[300];
        hospital.guiRemovePatient(tb[0].buf, result);
        showMsg(result, Screen::ADMIN_MENU);
    }
    if (drawBtn("Back", 410, 290, 130, 36, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//view all patients
void GUI::sViewAllPats(sf::Event* e)
{
    drawHeaderBar("All Patients"); drawTitle("All Patients", 70); drawSep(110);
    drawLabel("ID  | Name          | Age | G | Contact       | Balance  | Unpaid",
        25, 118, C_HDR, 14);
    drawSep(135);

    float y = 138 + scrollY; int ri = 0;
    for (int i = 0; i < hospital.getPatients().size(); i++) {
        Patient* p = hospital.getPatients().getDataAtIndex(i);
        if (!p) continue;
        int ub = 0;
        for (int j = 0; j < hospital.getBills().size(); j++) {
            Bill* b = hospital.getBills().getDataAtIndex(j);
            if (b && b->getPatientID() == p->getID() && hospital.strEq(b->getStatus(), "unpaid")) ub++;
        }
        if (y < 136 || y>570) { y += 28; ri++; continue; }

        char line[250], idBuf[10], ageBuf[6], balBuf[20], ubBuf[5], gBuf[3];
        istr(p->getID(), idBuf); istr(p->getAge(), ageBuf);
        fstr(p->getBalance(), balBuf); istr(ub, ubBuf);
        gBuf[0] = p->getGender(); gBuf[1] = '\0';

        cpy(line, idBuf); cat(line, "   | "); cat(line, p->getName());
        cat(line, "  | "); cat(line, ageBuf);
        cat(line, "  | "); cat(line, gBuf);
        cat(line, " | "); cat(line, p->getContact());
        cat(line, " | "); cat(line, balBuf);
        cat(line, "  | "); cat(line, ubBuf);

        sf::RectangleShape row({ 780,26 }); row.setPosition(20, y);
        row.setFillColor(ri % 2 == 0 ? C_ROW_A : C_ROW_B); window.draw(row);
        drawLabel(line, 28, y + 4, C_TEXT, 13);
        y += 28; ri++;
    }
    if (drawBtn("Back", 330, 582, 160, 30, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//view all doctors
void GUI::sViewAllDocs(sf::Event* e)
{
    drawHeaderBar("All Doctors"); drawTitle("All Doctors", 70); drawSep(110);
    drawLabel("ID  | Name              | Specialization    | Contact       | Fee",
        25, 118, C_HDR, 14);
    drawSep(135);

    float y = 138 + scrollY; int ri = 0;
    for (int i = 0; i < hospital.getDoctors().size(); i++) {
        Doctor* d = hospital.getDoctors().getDataAtIndex(i);
        if (!d) continue;
        if (y < 136 || y>570) { y += 28; ri++; continue; }

        char line[250], idBuf[10], feeBuf[20];
        istr(d->getID(), idBuf); fstr(d->getFee(), feeBuf);
        cpy(line, idBuf); cat(line, "   | "); cat(line, d->getName());
        cat(line, "  | "); cat(line, d->getSpecialization());
        cat(line, "  | "); cat(line, d->getContact());
        cat(line, " | "); cat(line, feeBuf);

        sf::RectangleShape row({ 780,26 }); row.setPosition(20, y);
        row.setFillColor(ri % 2 == 0 ? C_ROW_A : C_ROW_B); window.draw(row);
        drawLabel(line, 28, y + 4, C_TEXT, 13);
        y += 28; ri++;
    }
    if (drawBtn("Back", 330, 582, 160, 30, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//view all appointments
void GUI::sViewAllAppts(sf::Event* e)
{
    drawHeaderBar("All Appointments"); drawTitle("All Appointments", 70); drawSep(110);
    drawLabel("ID  | Patient        | Doctor         | Date       | Slot  | Status",
        25, 118, C_HDR, 14);
    drawSep(135);

    float y = 138 + scrollY; int ri = 0;
    for (int i = 0; i < hospital.getAppointments().size(); i++) {
        Appointment* a = hospital.getAppointments().getDataAtIndex(i);
        if (!a) continue;
        if (y < 136 || y>570) { y += 26; ri++; continue; }

        Patient* pt = hospital.getPatients().findByID(a->getPatientID());
        Doctor* d = hospital.getDoctors().findByID(a->getDoctorID());
        char line[250], idBuf[10]; istr(a->getApptID(), idBuf);
        cpy(line, idBuf); cat(line, "   | ");
        cat(line, pt ? pt->getName() : "N/A"); cat(line, "  | ");
        cat(line, d ? d->getName() : "N/A");  cat(line, "  | ");
        cat(line, a->getDate()); cat(line, "  | ");
        cat(line, a->getTimeSlot()); cat(line, "  | ");
        cat(line, a->getStatus());

        sf::RectangleShape row({ 780,24 }); row.setPosition(20, y);
        row.setFillColor(ri % 2 == 0 ? C_ROW_A : C_ROW_B); window.draw(row);
        drawLabel(line, 28, y + 3, C_TEXT, 13);
        y += 26; ri++;
    }
    if (drawBtn("Back", 330, 582, 160, 30, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//view the unpaid bills
void GUI::sViewUnpaid(sf::Event* e)
{
    drawHeaderBar("Unpaid Bills"); drawTitle("Unpaid Bills", 70); drawSep(110);
    drawLabel("Bill | Patient           | Amount (PKR) | Date", 25, 118, C_HDR, 15);
    drawSep(136);

    char today[11]; hospital.getTodayStr(today);
    float y = 140 + scrollY; bool any = false; int ri = 0;
    for (int i = 0; i < hospital.getBills().size(); i++) {
        Bill* b = hospital.getBills().getDataAtIndex(i);
        if (!b || !hospital.strEq(b->getStatus(), "unpaid")) continue;
        any = true;
        if (y < 138 || y>570) { y += 32; ri++; continue; }

        Patient* pt = hospital.getPatients().findByID(b->getPatientID());
        bool overdue = hospital.isOverdue(b->getDate(), today);
        char line[200], bBuf[10], amtBuf[20];
        istr(b->getBillID(), bBuf); fstr(b->getAmount(), amtBuf);
        cpy(line, bBuf); cat(line, "    | ");
        cat(line, pt ? pt->getName() : "N/A");
        cat(line, "  | "); cat(line, amtBuf);
        cat(line, "  | "); cat(line, b->getDate());
        if (overdue) cat(line, " [OVERDUE]");

        sf::RectangleShape row({ 780,30 }); row.setPosition(20, y);
        row.setFillColor(ri % 2 == 0 ? C_ROW_A : C_ROW_B); window.draw(row);
        drawLabel(line, 28, y + 6, overdue ? C_RED : C_TEXT, 14);
        y += 32; ri++;
    }
    if (!any) drawLabel("No unpaid bills.", 160, 230, C_TEXT);
    if (drawBtn("Back", 330, 578, 160, 34, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//discharge a patient
void GUI::sDischarge(sf::Event* e)
{
    tb[0].isPass = false;
    tb[0].box.setPosition(260, 230); tb[0].box.setSize({ 300,34 }); tb[0].refresh();
    handleTBs(e, 1);

    drawHeaderBar("Discharge Patient"); drawTitle("Discharge Patient", 70); drawSep(110);
    drawLabel("Enter Patient ID:", 160, 202, C_TEXT);
    tb[0].draw(window);

    if (drawBtn("Discharge", 240, 290, 160, 36, e, C_RED)) {
        char result[300];
        hospital.guiDischarge(tb[0].buf, result);
        showMsg(result, Screen::ADMIN_MENU);
    }
    if (drawBtn("Back", 420, 290, 130, 36, e, C_GRAY)) goTo(Screen::ADMIN_MENU);
}

//security log
void GUI::sSecLog(sf::Event* e)
{
    drawHeaderBar("Security Log"); drawTitle("Security Log", 70); drawSep(110);
    if (listBuf[0] == '\0') hospital.guiSecurityLog(listBuf, 8000);
    drawScrollText(listBuf, 120);
    if (drawBtn("Back", 330, 582, 160, 30, e, C_GRAY)) { listBuf[0] = '\0'; goTo(Screen::ADMIN_MENU); }
}

//daily report
void GUI::sDailyReport(sf::Event* e)
{
    drawHeaderBar("Daily Report"); drawTitle("Daily Report", 70); drawSep(110);
    if (listBuf[0] == '\0') hospital.guiDailyReport(listBuf, 8000);
    drawScrollText(listBuf, 120);
    if (drawBtn("Back", 330, 582, 160, 30, e, C_GRAY)) { listBuf[0] = '\0'; goTo(Screen::ADMIN_MENU); }
}

//message screen
void GUI::sMessage(sf::Event* e)
{
    drawHeaderBar(); drawTitle("Result", 70); drawSep(110);
    drawScrollText(msgBuf, 130);
    if (drawBtn("OK", 330, 565, 160, 40, e)) goTo(prev);
}