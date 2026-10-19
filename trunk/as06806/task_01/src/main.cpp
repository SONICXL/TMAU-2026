#include <iostream>
#include <fstream>
#include <locale>
#include <iomanip>
#include <windows.h>
#include <memory>

#include "model.h"

using namespace std;

int main(){
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int n;
    int signalChoice;
    int modelChoice;
    
    cout << "Введите количество шагов моделирования: ";
    if(!(cin >> n)  || n <= 0) { 
        cout << "Ошибка количество шагов должно быть положительным числом.";
        return 1;
    }

    cout << "\nВыберите модель:\n";
    cout << "1 - Модель 1.9 (линейная)\n";
    cout << "2 - Модель 2.1 (нелинейная)\n";
    cout << "3 - Модель 3.3 (дифференциальная)\n";
    cout << "Ваш выбор: ";

    if (!(cin >> modelChoice) || modelChoice < 1 || modelChoice > 3) {
        cout << "Ошибка: выберите модель от 1 до 3.\n";
        return 1;
    }

    unique_ptr<Model> model;

    string modelCsvName;

    if (modelChoice == 1) {
        double a1, a2, a3, b1;

        cout << "\nНастройка модели 1.9\n";

        cout << "Введите коэффициент a1: ";
        if (!(cin >> a1)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        cout << "Введите коэффициент a2: ";
        if (!(cin >> a2)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        cout << "Введите коэффициент a3: ";
        if (!(cin >> a3)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        cout << "Введите коэффициент b1: ";
        if (!(cin >> b1)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        model = make_unique<Model1_9>(a1, a2, a3, b1);
        modelCsvName = "Model 1.9";
    }

    else if (modelChoice == 2) {
        double a, b, c, d;

        cout << "\nНастройка модели 2.1\n";

        cout << "Введите коэффициент a: ";
        if (!(cin >> a)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        cout << "Введите коэффициент b: ";
        if (!(cin >> b)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        cout << "Введите коэффициент c: ";
        if (!(cin >> c)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        cout << "Введите коэффициент d: ";
        if (!(cin >> d)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        model = make_unique<Model2_1>(a, b, c, d);
        modelCsvName = "Model 2.1";
    }

    else {
        double a, b, deltaT;

        cout << "\nНастройка модели 3.3\n";

        cout << "Введите коэффициент a: ";
        if (!(cin >> a)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        cout << "Введите коэффициент b: ";
        if (!(cin >> b)) {
            cout << "Ошибка: необходимо ввести число.\n";
            return 1;
        }

        cout << "Введите шаг времени deltaT: ";
        if (!(cin >> deltaT) || deltaT <= 0.0) {
            cout << "Ошибка: шаг времени должен быть больше нуля.\n";
            return 1;
        }

        model = make_unique<Model3_3>(a, b, deltaT);
        modelCsvName = "Model 3.3";
    }


    cout << "\nВыберите входное воздействие:\n";
    cout << "1 - Ступенчатое\n";
    cout << "2 - Импульсное\n";
    cout << "3 - Гармоническое\n";

    if(!(cin >> signalChoice) || signalChoice < 1 || signalChoice > 3) {
        cout << "Ошибка: выберите вариант от 1 до 3.\n";
        return 1;
    }

    InputSignals signalType = static_cast<InputSignals>(signalChoice);

    string signalCsvName;

    switch(signalType){

    case InputSignals::Step:
        signalCsvName = "Step";
        break;

    case InputSignals::Impulse:
        signalCsvName = "Impulse";
        break;

    case InputSignals::Harmonic:
        signalCsvName = "Harmonic";
        break;
    }

    ofstream csv("result.csv", ios::out | ios::trunc);
    csv.imbue(locale::classic());

    if(!csv.is_open()){
        cout << "Ошибка: не удалось создать result.csv.\n";
        return 1;
    }

    csv << "step,u_tau,y_next,model,signal\n";
    csv << setprecision(15);


    cout << "\nМодель: " << model->getName() << "\n\n";

    cout << setw(13) << "Шаг"
         << setw(15) << "u(tau)"
         << setw(15) << "y(tau+1)" << "\n";
    cout << string(40, '-') << "\n";

    cout << fixed << setprecision(4);

    for (int tau = 0; tau < n; ++tau){
        double utau = calculateInput(tau, signalType);
        double y = model->calculateNextStep(utau);
        if(!isfinite(y) || fabs(y) > 1e9){
            cout << "Ошибка: расчёт расходится (|y| > 1e9) на шаге " << tau + 1 << ". Измените коэффициенты модели.\n";
            break;
        }

        cout << setw(10) << tau + 1
             << setw(15) << utau
             << setw(15) << y << '\n';

        csv << tau + 1 << ','
            << utau << ','
            << y << ','
            << modelCsvName << ','
            << signalCsvName << '\n';
    }

    csv.close();

    cout << "\nРезультаты сохранены в result.csv\n";
    
    return 0;
}