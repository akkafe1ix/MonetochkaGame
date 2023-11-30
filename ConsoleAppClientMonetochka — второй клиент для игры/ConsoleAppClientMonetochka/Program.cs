using System;
using System.Net.Sockets;
using System.Text;

class Program
{
    static void Main()
    {
        try
        {
            // Устанавливаем IP адрес и порт сервера
            string serverIP = "82.179.140.18"; // IP адрес сервера
            int serverPort = 45693; // Порт сервера

            // Создаем TcpClient
            TcpClient client = new TcpClient(serverIP, serverPort);

            Console.WriteLine("Подключено к серверу...");

            // Получаем поток для чтения и записи
            NetworkStream stream = client.GetStream();

            byte[] responseData = new byte[1024];
            int bytesRead = stream.Read(responseData, 0, responseData.Length);
            string response = Encoding.UTF8.GetString(responseData, 0, bytesRead);

            Console.WriteLine(response);
            string player = response[8].ToString();

            while (true)
            {
                if (((response == "Угадайте в какую руку противник положил монетку (1/2)") & (player == "1")) | ((response == "Сделайте выбор в какую руку положить монетку (1/2)") & (player == "2")))
                {
                    Console.Write("Ответ: ");
                    string message = Console.ReadLine();

                    //Конвертируем сообщение в массив байтов
                    byte[] data = Encoding.UTF8.GetBytes(message);

                    //Отправляем сообщение на сервер
                    stream.Write(data, 0, data.Length);
                }

                // Получаем ответ от сервера
                responseData = new byte[1024];
                bytesRead = stream.Read(responseData, 0, responseData.Length);
                response = Encoding.UTF8.GetString(responseData, 0, bytesRead);
                if (response != "1")
                {
                    Console.WriteLine("Ведущий: " + response);
                }
            }

            // Закрываем соединение
            stream.Close();
            client.Close();
        }
        catch (Exception e)
        {
            Console.WriteLine("Ошибка: " + e.Message);
        }
    }
}
