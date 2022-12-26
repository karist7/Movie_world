#include<iostream>
#include <string>
#include "sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

using namespace std;

//db사용
sqlite3* db;
//에러 메세지
char* ErrMsg = 0;
//db명령어 exec 실행
int rc;
//조회시 임시 저장 배열
string a[5];
//string 연결
string quotesql(const string& s) {
    return string("'") + s + string("'");
}
//조회 후 배열에 저장하며 정보 출력
static int callback(void* data, int argc, char** argv, char** azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "인기") == 0)
            cout << "★" << "\t\t";
        else if (strcmp(argv[i], "비인기") == 0)
            cout << "☆" << "\t\t";
        else
            cout << (argv[i] ? argv[i] : "NULL") << "\b\t\t";
        a[i] = (argv[i] ? argv[i] : "NULL");

    }
    cout << endl;
    return 0;
}
//조회후 배열에만 저장
static int callback2(void* data, int argc, char** argv, char** azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        a[i] = (argv[i] ? argv[i] : "NULL");

    }
    return 0;
}

class Movie
{
    string premium;
    string name;
    string teaser;
    string is_rented;

public:
    Movie() {};
    Movie(string name, string teaser, string premium, string is_rented = "대여 가능") : name(name), teaser(teaser), premium(premium), is_rented(is_rented) {};

    string getName()
    {
        return (name);
    }

    string getIs_rented()
    {
        return (is_rented);
    }
    string getTeaser() {
        return teaser;
    }
    string getPremium() {
        return premium;
    }
};


class Account
{
    string id;
    string pwd;
public:
    Account(string id, string pwd) : id(id), pwd(pwd) {};
    //계정간 조회용 순수 가상 함수
    virtual void display() = 0;
    string getId()
    {
        return (id);
    }
    string getPwd()
    {
        return (pwd);
    }


};

class Administrator : public Account
{

public:
    Administrator(string id, string pwd) : Account(id, pwd) {  };
    //조회
    void display() {
        cout << "관리자 명령어: 추가: 1, 삭제: 2, 종료: 3" << endl;
        cout << "영화제목\t\b\b\b\b인기영화\t\t티저 영상 링크\t\t\t\t\t\t대여 가능 여부" << endl;
        string sqlstatement =
            "select * from movie ";
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, callback, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }

    }
    //영화 추가
    void insertMovie(Movie movie) {
        string sqlstatement =
            "INSERT INTO movie (TITLE,PREMIUM,TEASER,is_RENT) VALUES ("
            + quotesql(movie.getName()) + ","
            + quotesql(movie.getPremium()) + ","
            + quotesql(movie.getTeaser()) + ","
            + quotesql(movie.getIs_rented()) + ");";
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, NULL, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
    }
    //영화 삭제
    void deleteMovie(string name)
    {
        string sqlstatement =
            "DELETE FROM movie WHERE title = " + quotesql(name);
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, NULL, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
    }


};

class User : public Account
{
    string membership;
    unsigned long long amount;
    string rental;

public:

    User(string id, string pwd, int amount = 0, string rental = "", string membership = "일반") : Account(id, pwd)
    {
        this->membership = membership;
        this->amount = amount;
        this->rental = rental;
    };
    //조회 페이지
    void display() {
        cout << "★=인기영화: 금액 4000원  " << endl;
        cout << "☆=비인기영화: 금액 2000원 " << endl;
        cout << "골드 회원 10% 할인" << endl;
        cout << "영화제목\t\b\b\b\b인기영화\t\t티저 영상 링크\t\t\t\t\t\t대여 가능 여부" << endl;
        string sqlstatement =
            "select * from movie ";
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, callback, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
    }
    //마이 페이지
    void myPage() {
        cout << "내 정보" << endl;
        cout << "id\t\tMembership\tAmount\t\tRental" << endl;
        string sqlstatement =
            "select id,Membership,amount,rental from user where id=" + quotesql(getId());
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, callback, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
    }
    //계정 생성
    void create_account() {
        string sqlstatement =
            "INSERT INTO user (id,password,membership,amount,rental) VALUES ("
            + quotesql(getId()) + ","
            + quotesql(getPwd()) + ","
            + quotesql(membership) + ","
            + quotesql(to_string(amount)) + ","
            + quotesql(rental) + ");";
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, NULL, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
    }
    //금액 충전
    void charge(int won) {
        string sqlstatement =
            "select amount from user WHERE id = " + quotesql(getId());
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, callback2, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
        int total = stoi(a[0]) + won;
        string sqlstatement2 =
            "UPDATE user set amount = " + quotesql(to_string(total)) + " WHERE id = " + quotesql(getId());
        const char* sql2 = sqlstatement2.c_str();
        rc = sqlite3_exec(db, sql2, NULL, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }

    }

    //멤버쉽 변경
    void changeMembetship() {
        string s =
            "SELECT * FROM user where id=" + quotesql(getId());

        const char* Lists = s.c_str();
        rc = sqlite3_exec(db, Lists, callback2, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }

        if (a[2].compare("일반") == 0) {
            try {
                int total = stoi(a[3]) - 8000;
                if (total < 0)
                    throw total;
                string usertotal =
                    "UPDATE user set amount = " + quotesql(to_string(total)) + " WHERE id = " + quotesql(getId());
                const char* users = usertotal.c_str();
                rc = sqlite3_exec(db, users, NULL, NULL, &ErrMsg);
                string updateMembership =
                    "UPDATE user set membership = " + quotesql("골드") + " WHERE id = " + quotesql(getId());
                const char* update = updateMembership.c_str();
                rc = sqlite3_exec(db, update, NULL, NULL, &ErrMsg);
                cout << "회원등급이 골드로 전환되었습니다." << endl;
            }
            catch (int e) {
                cout << "금액이 부족합니다." << endl;
            }
        }
        else {
            string updateMembership =
                "UPDATE user set membership = " + quotesql("일반") + " WHERE id = " + quotesql(getId());
            const char* update = updateMembership.c_str();
            rc = sqlite3_exec(db, update, NULL, NULL, &ErrMsg);
            cout << "회원 등급이 일반으로 전환되었습니다." << endl;
        }

    }
    //회원 탈퇴
    void DeleteMember(string id) {
        string sqlstatement =
            "DELETE FROM user WHERE id = " + quotesql(id);
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, NULL, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
    }
    //렌트 후 가격 계산, 돈이 충분한지를 알기 위해 예외처리 이용
    bool borrow(string name) {
        int total;
        //영화의 프리미엄 등급 조회
        string sqlstatement =
            "select premium from movie WHERE title = " + quotesql(name);
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, callback2, NULL, &ErrMsg);
        //비인기 영화인 경우

        if (a[1].compare("비인기") == 0) {

            string userstate =
                "select * from user WHERE id = " + quotesql(getId());
            const char* useramount = userstate.c_str();
            rc = sqlite3_exec(db, useramount, callback2, NULL, &ErrMsg);

            if (a[2].compare("일반") == 0) {

                try {
                    total = stoi(a[3]) - 2000;
                    if (total < 0) {
                        throw total;

                    }

                    string usertotal =
                        "UPDATE user set amount = " + quotesql(to_string(total)) + " WHERE id = " + quotesql(getId());

                    const char* users = usertotal.c_str();
                    rc = sqlite3_exec(db, users, NULL, NULL, &ErrMsg);

                }
                catch (int e) {
                    cout << "금액이 부족합니다." << endl;
                    return false;
                }
            }
            //골드 회원
            else {
                try {

                    total = stoi(a[3]) - 1800;
                    if (total < 0) {
                        throw total;
                    }
                    cout << total << endl;
                    string usertotal2 =
                        "UPDATE user set amount = " + quotesql(to_string(total)) + " WHERE id = " + quotesql(getId());
                    const char* users2 = usertotal2.c_str();
                    rc = sqlite3_exec(db, users2, NULL, NULL, &ErrMsg);

                }
                catch (int e) {
                    cout << "금액이 부족합니다." << endl;
                    return false;
                }
            }

        }
        //인기 영화의 경우
        else {

            string userstate2 =
                "select * from user WHERE id = " + quotesql(getId());
            const char* useramount2 = userstate2.c_str();
            rc = sqlite3_exec(db, useramount2, callback2, NULL, &ErrMsg);

            if (a[2].compare("일반") == 0) {
                try {
                    total = stoi(a[3]) - 4000;
                    if (total < 0) {
                        throw total;

                    }
                    string usertotal2 =
                        "UPDATE user set amount = " + quotesql(to_string(total)) + " WHERE id = " + quotesql(getId());
                    const char* users2 = usertotal2.c_str();
                    rc = sqlite3_exec(db, users2, NULL, NULL, &ErrMsg);

                }
                catch (int e) {
                    cout << "금액이 부족합니다." << endl;
                    return false;
                }
            }
            //골드 회원
            else {
                try {
                    total = stoi(a[3]) - 3600;
                    if (total < 0) {
                        throw total;
                    }
                    string usertotal2 =
                        "UPDATE user set amount = " + quotesql(to_string(total)) + " WHERE id = " + quotesql(getId());
                    const char* users2 = usertotal2.c_str();
                    rc = sqlite3_exec(db, users2, NULL, NULL, &ErrMsg);
                }
                catch (int e) {
                    cout << "금액이 부족합니다." << endl;
                    return false;
                }
            }
        }
        return true;
    }
    //렌트
    bool rent(string dvd) {
        string sqlstatement =
            "select * from movie where title= " + quotesql(dvd);
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, callback2, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }

        if (a[3].compare("대여 불가능") == 0 || !borrow(dvd)) {
            return false;
        }
        else {

            cout << dvd + "영화를 빌렸습니다." << endl;
            string sqlstatement2 =
                "UPDATE movie set is_RENT = " + quotesql("대여 불가능") + " WHERE title = " + quotesql(dvd);
            const char* sql2 = sqlstatement2.c_str();
            rc = sqlite3_exec(db, sql2, NULL, NULL, &ErrMsg);
            string sqlstatement3 =
                "UPDATE user set RENTAL = " + quotesql(dvd) + " WHERE id = " + quotesql(getId());
            const char* sql3 = sqlstatement3.c_str();
            rc = sqlite3_exec(db, sql3, NULL, NULL, &ErrMsg);
            return true;
        }
    }
    //반납
    void Return_Movie() {
        cout << endl;
        string dvd;
        string sqlstatement =
            "select * from user where id= " + quotesql(getId());
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, callback2, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
        cout << "반납 dvd: " << a[4] << endl;
        dvd = a[4];

        string sqlstatement3 =
            "UPDATE user set RENTAL = " + quotesql(" ") + " WHERE id = " + quotesql(getId());
        const char* sql3 = sqlstatement3.c_str();
        rc = sqlite3_exec(db, sql3, NULL, NULL, &ErrMsg);
        string sqlstatement2 =
            "UPDATE movie set is_RENT = " + quotesql("대여 가능") + " WHERE title = " + quotesql(dvd);
        const char* sql2 = sqlstatement2.c_str();
        rc = sqlite3_exec(db, sql2, NULL, NULL, &ErrMsg);
    }
    //검색
    void search(string name) {
        cout << "영화제목\t\b\b\b\b인기영화\t\t티저 영상 링크\t\t\t\t\t\t대여 가능 여부" << endl;
        string sqlstatement =
            "select * from movie where title= " + quotesql(name);
        const char* sql = sqlstatement.c_str();
        rc = sqlite3_exec(db, sql, callback, NULL, &ErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error: " << ErrMsg << endl;
            sqlite3_free(ErrMsg);
        }
        if (a[0].compare(name) != 0) {
            cout << "해당 영화를 찾을 수 없습니다.";
            return;
        }
    }
};


//테이블 생성 함수
void create_table(const char* sql) {
    rc = sqlite3_exec(db, sql, NULL, NULL, &ErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << ErrMsg << endl;
        sqlite3_free(ErrMsg);
    }
}

//-1이면 로그인 실패 0이면 관리자 1이면 이용자
int login(string id, string pwd)
{
    string IDinfo =
        "select * from user WHERE id = " + quotesql(id);
    const char* info = IDinfo.c_str();
    rc = sqlite3_exec(db, info, callback2, NULL, &ErrMsg);
    if (a[0].compare("admin") == 0 && a[1].compare("1234") == 0) {
        return 0;

    }
    else if (a[0].compare(id) == 0 && a[1].compare(pwd) == 0) {
        return 1;
    }
    else {
        return -1;

    }

}

int main()
{
    //db open
    rc = sqlite3_open("MovieWorld.db", &db);
    if (rc == SQLITE_OK) {
        cout << "Database opened successfully." << endl;
    }
    else {
        cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return -1;
    }
    //테이블 생성문
    const char* create_user = "CREATE TABLE if not exists USER(" \
        "ID TEXT PRIMARY KEY, " \
        "PASSWORD TEXT NOT NULL, " \
        "MEMBERSHIP TEXT NOT NULL, " \
        "AMOUNT INTEGER NOT NULL, " \
        "RENTAL TEXT); ";
    create_table(create_user);
    const char* create_admin = "CREATE TABLE if not exists admin(" \
        "ID TEXT PRIMARY KEY, " \
        "PASSWORD TEXT NOT NULL, " \
        "INCOME TEXT); ";
    create_table(create_admin);
    const char* create_list = "CREATE TABLE if not exists Movie(" \
        "TITLE TEXT PRIMARY KEY, " \
        "PREMIUM TEXT NOT NULL, " \
        "TEASER TEXT NOT NULL, " \
        "IS_RENT TEXT NOT NULL); ";
    create_table(create_list);


    int n;
    int i = 0;
    //acount
    string id = "", pwd = "";
    //movie
    string title = "", teaser = "", premium = "";
    int check = 0;
    cout << "Movie World에 오신 것을 환영합니다\n";

    while (1)
    {
        try //올바른 숫자 미입력시 throw
        {
            cout << "1.로그인\n2.회원가입\n숫자를 입력해주세요: ";
            cin >> i;
            if (cin.fail())
                throw i;
        }
        catch (int e)
        {
            cout << "잘못된 입력입니다. 정수를 입력해주세요.";
            continue;
        }
        if (i == 1) {
            break;
        }
        //회원가입    
        else if (i == 2)
        {
            cout << "ID를 입력해주세요: ";
            cin >> id;
            cout << "비밀번호를 입력해주세요: ";
            cin >> pwd;
            User p(id, pwd);
            p.create_account();
            cout << "회원가입을 완료했습니다." << endl;
            cout << endl;
        }
        else
            cout << "잘못된 입력입니다.\n";
    }

    while (1)
    {
        //로그인 시도
        //ID 미입력시 예외 발생
        try
        {
            cout << "ID를 입력해주세요: ";
            cin >> id;
            if (id.compare("") == 0)
                throw id;
        }
        catch (string e)
        {
            cout << "아무것도 입력되지 않았습니다.\n";
            continue;
        }
        cout << "비밀번호를 입력해주세요: ";
        cin >> pwd;
        //로그인 시도
        n = login(id, pwd);
        //-1이면 로그인 실패
        if (n != -1)
            break;
        else
            cout << "잘못된 ID 혹은 비밀번호입니다. 다시 입력해주세요.\n";
    }

    //관리자
    if (n == 0) {


        Account* admin = new Administrator(id, pwd);
        admin->display();
        Administrator* adminstrator = (Administrator*)admin;
        cout << endl;
        while (1) {

            cout << "명령 선택: ";
            cin >> check;
            switch (check)
            {
            case 1:
                cout << "영화 제목: ";
                cin >> title;
                cout << "티저 링크: ";
                cin >> teaser;
                cout << "인기도: ";
                cin >> premium;
                adminstrator->insertMovie(Movie(title, teaser, premium));
                cout << endl;
                adminstrator->display();
                break;
            case 2:
                cout << "영화 제목: ";
                cin >> title;
                adminstrator->deleteMovie(title);
                cout << endl;
                adminstrator->display();
                break;
            case 3:
                cout << "프로그램을 종료합니다." << endl;
                exit(0);
            default:
                cout << "잘못된 명령어 입력." << endl;
                break;
            }
        }
    }
    //사용자
    else {
        int instruction = 0;
        Account* user = new User(id, pwd);

        while (1) {
            cout << "\n조회: 1번, 마이페이지: 2번, 종료: 3번: ";
            cin >> check;
            if (check == 1) {
                while (1) {
                    cout << endl;
                    user->display();
                    User* p = (User*)user;
                    cout << "명령을 입력하시오: 충전: 1번, 렌탈: 2번, 검색: 3번, 이전: 4번: ";
                    cin >> instruction;
                    if (instruction == 1) {
                        int money;
                        cout << "충전할 금액을 입력하시오: ";
                        cin >> money;
                        p->charge(money);
                    }
                    else  if (instruction == 2) {
                        cout << endl;
                        cout << "렌트할 영화 제목을 입력하시오: ";
                        cin >> title;
                        if (!p->rent(title)) {
                            cout << "해당 영화는 존재하지 않거나 빌릴 수 없습니다." << endl;
                        }
                        else {

                            cout << "렌트 완료했습니다. 기한은 1주일 입니다.";
                        }

                    }
                    else if (instruction == 3) {
                        cout << endl;
                        cout << "검색할 영화 제목을 입력하시오: ";
                        cin >> title;
                        p->search(title);
                    }
                    else {

                        cout << "이전으로 이동합니다." << endl;
                        break;
                    }
                }
            }
            else if (check == 2) {
                while (1) {
                    User* p = (User*)user;
                    p->myPage();
                    cout << "\n명령을 입력하십시오: 회원 등급 전환: 1번, 회원탈퇴: 2번, 반납: 3번 이전: 4번: ";
                    cin >> instruction;
                    cout << endl;
                    if (instruction == 1) {
                        p->changeMembetship();

                    }
                    else if (instruction == 2) {
                        p->DeleteMember(p->getId());
                        cout << "회원탈퇴가 완료되었습니다. 이용해주셔서 감사합니다." << endl;
                        exit(0);
                    }
                    else if (instruction == 3) {
                        p->Return_Movie();
                        cout << "반납이 완료되었습니다." << endl;

                    }
                    else {
                        cout << "이전으로 이동합니다." << endl;
                        break;
                    }
                }
            }
            else {
                cout << "프로그램을 종료합니다.";
                break;
            }
        }
    }
    sqlite3_close(db);
    return (0);
}
