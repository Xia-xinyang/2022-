#include "login.h"
#include "ui_login.h"
#include<iostream>
#include "component.h"
#include "vector"
#include "QMessageBox"

using namespace std;

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->setWindowTitle("管理员登录");
}

login::~login()
{
    delete ui;
}

void login::on_pushButton_clicked()//登录按钮
{
    string username = ui->username->text().toStdString();
    string password = ui->password->text().toStdString();

    vector<User>adminList;//登录账号、密码
    adminList.push_back(User("root","root",true));//管理员1
    adminList.push_back(User("admin","admin",true));//管理员2
    adminList.push_back(User("stu","abcdef",false));//学生1
    adminList.push_back(User("stu","abcdef",false));//学生2
    for (auto obj : adminList)
    {
        if(obj.username==username&&obj.password==password){
            this->indexPage = new dashboard(obj.isRoot);
            this->indexPage->show();
            this->close();
            return;
        };
    }
    QMessageBox::about(NULL, "错误", "账号或密码错误，请重试！");
}

User::User(string username0,string password0,bool isRoot0):username(username0),password(password0),isRoot(isRoot0){};
