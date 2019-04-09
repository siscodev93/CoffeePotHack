using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;


namespace coffee_maker
{
    class Program
    {
        static TcpClient client;
        static NetworkStream stream;

        static void Connect(String ip, Int32 port)
        {
            if (client == null)
            {
                client = new TcpClient(ip, port);
                stream = client.GetStream();
            }
            else
            {
                Console.WriteLine("Error, TcpClient Already Connected");
            }
        }
        static void Disconnect()
        {
            client.Close();
            stream.Close();
        }
        
        static void SendPacket(String cmd)
        {
            Console.WriteLine("Sending Packet to Coffee Machine");

            Byte[] data = System.Text.Encoding.ASCII.GetBytes(cmd);
            stream.Write(data, 0, data.Length);

            data = new byte[50];
            String response = String.Empty;
            Int32 bytes = stream.Read(data, 0, data.Length);
            response = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
            Console.WriteLine("Response: {0}", response);

        }
        static void Main(string[] args)
        {
            if(args.Length < 3)
            {
                Console.WriteLine("coffee-maker [address] [port] [cmd]");
                return;
            }
            int port;
            Int32.TryParse(args[1], out port);
            Connect(args[0], port);

            SendPacket(args[2]);

            Disconnect();
        }
    }
}
