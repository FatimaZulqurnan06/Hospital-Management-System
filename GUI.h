#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include "Hospital.h"

enum class Screen
{
    MAIN_LOGIN,
    PATIENT_LOGIN, DOCTOR_LOGIN, ADMIN_LOGIN,
    PATIENT_MENU, DOCTOR_MENU, ADMIN_MENU,
    BOOK_SPEC, BOOK_DOCS, BOOK_DATE, BOOK_SLOTS,
    CANCEL_APPT, VIEW_MY_APPTS, VIEW_RECORDS,
    VIEW_BILLS, PAY_BILL, TOPUP,
    VIEW_TODAY, MARK_COMPLETE, MARK_NOSHOW,
    WRITE_PRESC, VIEW_PAT_HIST,
    ADD_DOCTOR, REMOVE_DOCTOR, ADD_PATIENT, REMOVE_PATIENT,
    VIEW_ALL_PATS, VIEW_ALL_DOCS, VIEW_ALL_APPTS,
    VIEW_UNPAID, DISCHARGE, SEC_LOG, DAILY_REPORT,
    MESSAGE
};

// Simple text input box
struct TBox
{
    sf::RectangleShape box;
    sf::Text txt;
    char buf[512];
    int len = 0;
    bool active = false;
    bool isPass = false;

    void init(sf::Font& f, float x, float y, float w, float h, bool pw = false);
    void clear();
    void handle(sf::Event& e);
    void refresh();
    bool hit(sf::Vector2f m) { return box.getGlobalBounds().contains(m); }
    void draw(sf::RenderWindow& w) { w.draw(box); w.draw(txt); }
};

class GUI
{
public:
    GUI();
    void run();

private:
    sf::RenderWindow window;
    sf::Font font;
    Hospital hospital;

    Screen cur, prev;
    float scrollY;

    Patient* logPat;
    Doctor* logDoc;
    int loginFails;

    // booking flow
    int  bookDocID;
    char bookSpec[60];
    char bookDate[20];

    char msgBuf[2000];
    char listBuf[8000];
    char errMsg[200];

    TBox tb[6]; //text boxes


    void goTo(Screen s);
    void showMsg(const char* m, Screen ret);


    bool wasClicked(sf::Event* e);
    sf::Vector2f mousePos();
    void activateTB(int which, sf::Vector2f m, int count);
    void handleTBs(sf::Event* e, int count);


    void drawHeaderBar(const char* subtitle = nullptr);
    void drawTitle(const char* t, float y, unsigned sz = 26);
    void drawLabel(const char* t, float x, float y,
    sf::Color c = sf::Color(30, 30, 30), unsigned sz = 18);
    void drawSep(float y);
    void drawScrollText(const char* text, float startY);


    bool drawBtn(const char* lbl, float x, float y, float w, float h,
    sf::Event* e, sf::Color col = sf::Color(50, 110, 180), sf::Color hover = sf::Color(80, 150, 220));
    bool drawRow(int rowIdx, float y, float rowH, const char* text, sf::Event* e, bool hasBtn = false, const char* btnLbl = "", sf::Color btnCol = sf::Color(50, 110, 180));


    void sMainLogin(sf::Event* e);
    void sPatientLogin(sf::Event* e);
    void sDoctorLogin(sf::Event* e);
    void sAdminLogin(sf::Event* e);

    void sPatientMenu(sf::Event* e);
    void sDoctorMenu(sf::Event* e);
    void sAdminMenu(sf::Event* e);

    void sBookSpec(sf::Event* e);
    void sBookDocs(sf::Event* e);
    void sBookDate(sf::Event* e);
    void sBookSlots(sf::Event* e);
    void sCancelAppt(sf::Event* e);
    void sViewMyAppts(sf::Event* e);
    void sViewRecords(sf::Event* e);
    void sViewBills(sf::Event* e);
    void sPayBill(sf::Event* e);
    void sTopUp(sf::Event* e);

    void sViewToday(sf::Event* e);
    void sMarkComplete(sf::Event* e);
    void sMarkNoShow(sf::Event* e);
    void sWritePresc(sf::Event* e);
    void sViewPatHist(sf::Event* e);

    void sAddDoctor(sf::Event* e);
    void sRemoveDoctor(sf::Event* e);
    void sAddPatient(sf::Event* e);
    void sRemovePat(sf::Event* e);
    void sViewAllPats(sf::Event* e);
    void sViewAllDocs(sf::Event* e);
    void sViewAllAppts(sf::Event* e);
    void sViewUnpaid(sf::Event* e);
    void sDischarge(sf::Event* e);
    void sSecLog(sf::Event* e);
    void sDailyReport(sf::Event* e);

    void sMessage(sf::Event* e);

    void cpy(char* d, const char* s) const;
    void cat(char* d, const char* s) const;
    int clen(const char* s) const;
    bool ceq(const char* a, const char* b) const;
    bool ceqCI(const char* a, const char* b) const;
    void istr(int n, char* b) const;
    void fstr(float f, char* b) const;
    int toi(const char* s) const;
};
#endif