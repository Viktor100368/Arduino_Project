// Видеообзоры и уроки работы с ARDUINO на YouTube-канале IOMOIO: https://www.youtube.com/channel/UCmNXABaTjX_iKH28TTJpiqA

#include <CustomStepper.h>            // Подключаем библиотеку управления шаговым двигателем. По умолчанию настроена на двигатель 28BYJ-48-5V
CustomStepper stepper(8, 9, 10, 11);  // Указываем пины, к которым подключен драйвер шагового двигателя

int task = 1;                      // Переменная для демонстрации работы, отвечающая за смену режимов

void setup()
{
  stepper.setRPM(12);                 // Устанавливаем кол-во оборотов в минуту
  stepper.setSPR(4075.7728395);       // Устанавливаем кол-во шагов на полный оборот. Максимальное значение 4075.7728395
}

void loop()
{
  if (stepper.isDone() && task ==1)  // Когда предыдущая команда выполнена (см. ниже), метод stepper.isDone() возвращает true
  {
    stepper.setDirection(CW);         // Устанавливает направление вращения. Может принимать 3 значения: CW - по часовой, CCW - против часовой, STOP
    stepper.rotate(2);                // Устанавливает вращение на заданное кол-во оборотов
    task=2;
  }
  if(stepper.isDone()){
    stepper.setDirection(CCW);  
    stepper.rotate(2);
    task = 1;
  }

  stepper.run();                      // Этот метод обязателен в блоке loop. Он инициирует работу двигателя, когда это необходимо
}
