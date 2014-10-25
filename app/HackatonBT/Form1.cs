using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using SlimDX.DirectInput;
using System.Globalization;

namespace HackatonBT
{

    

    public partial class Form1 : Form
    {
        public class GamepadDevice
        {
            public Guid Guid { get; set; }
            public string Name { get; set; }

            public override string ToString()
            {
                return Name;
            }
        }

        Joystick pad;
        GamepadDevice Device;

        Bitmap img;

        int pos = 70;

        IList<GamepadDevice> devices;

        Queue<string> data = new Queue<string>();
        object data_lock = new object();

        public Form1()
        {
            InitializeComponent();

            img = new Bitmap(pbRadar.Width, pbRadar.Height);

            Available();
        }

        private void acquire(System.Windows.Forms.Form parent)
        {
            DirectInput dinput = new DirectInput();

            pad = new Joystick(dinput, this.Device.Guid);
            foreach (DeviceObjectInstance doi in pad.GetObjects(ObjectDeviceType.Axis))
            {
                pad.GetObjectPropertiesById((int)doi.ObjectType).SetRange(-5000, 5000);
            }

            pad.Properties.AxisMode = DeviceAxisMode.Absolute;
            pad.SetCooperativeLevel(parent, (CooperativeLevel.Nonexclusive | CooperativeLevel.Background));
            pad.Acquire();
        }

        private void cbPort_DropDown(object sender, EventArgs e)
        {
            cbPort.DataSource = SerialPort.GetPortNames();
        }

        public virtual IList<GamepadDevice> Available()
        {
            IList<GamepadDevice> result = new List<GamepadDevice>();
            DirectInput dinput = new DirectInput();
            foreach (DeviceInstance di in dinput.GetDevices(DeviceClass.GameController, DeviceEnumerationFlags.AttachedOnly))
            {
                GamepadDevice dev = new GamepadDevice();
                dev.Guid = di.InstanceGuid;
                dev.Name = di.InstanceName;
                //Console.WriteLine(di.InstanceName) ;
                result.Add(dev);
            }
            return result;
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {

            if (serialPort.IsOpen)
            {
                serialPort.Close();
                btnConnect.Text = "Připojit";
            }
            else
            {
                if (cbPort.Text != "")
                {
                    serialPort.PortName = cbPort.Text;
                    serialPort.Open();
                    

                    btnConnect.Text = "Odpojit";
                }
            }


        }

        private void cbJoystick_DropDown(object sender, EventArgs e)
        {
            devices = Available();

            cbJoystick.DataSource = devices;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {

            if (Device != null)
            {
                JoystickState state = new JoystickState();

                if (pad.Poll().IsFailure)
                {
                    Console.WriteLine("fail");
                    return;
                }

                if (pad.GetCurrentState(ref state).IsFailure)
                {
                    Console.WriteLine("fail2");
                    return;
                }

                float speed = ((float)state.Z / -5000);
                float directionA = ((float)state.X / 5000);
                float directionB = ((float)state.RotationX / -5000);


                if (speed < 0)
                {
                    directionA *= -1;
                }

                if (Math.Abs(directionB) < 0.2)
                {
                    directionB = 0;
                }

                if (Math.Abs(directionA) < 0.4)
                {
                    directionA = 0;
                }

                if (Math.Abs(speed) < 0.2)
                {
                    speed = 0;
                }

                //pbSpeed.Value = (int)Math.Abs(speed * 100);
                //pbSpeed.ForeColor = (speed < 0 ? Color.Blue : Color.Red);

                bool[] buttons = state.GetButtons();
                bool prava = buttons[1];
                bool leva = buttons[2];
                bool reset = buttons[3];

                if (prava)
                {
                    pos -= 5;
                }
                else if (leva)
                {
                    pos += 5;
                }

                if (reset)
                {
                    pos = 70;
                }

                if (serialPort.IsOpen)
                {
                    if (directionA != 0 && speed == 0)
                    {
                        float l = directionA;
                        float r = directionA * -1;
                        string cmd = "M " + l.ToString("F", CultureInfo.InvariantCulture) + " " + r.ToString("F", CultureInfo.InvariantCulture) + "\n";
                        serialPort.WriteLine(cmd);
                        Console.WriteLine(cmd);
                    }
                    else if (speed != 0)
                    {
                        string cmd = "R " + directionB.ToString("F", CultureInfo.InvariantCulture) + " " + speed.ToString("F", CultureInfo.InvariantCulture) + "\n";
                        serialPort.WriteLine(cmd);
                        Console.WriteLine(cmd);
                    }
                    else
                    {
                        string cmd = "M 0 0\n";
                        serialPort.WriteLine(cmd);
                        Console.WriteLine(cmd);
                        cmd = "R 0 0\n";
                        serialPort.WriteLine(cmd);
                        Console.WriteLine(cmd);
                    }

                    string cmds = "C " + pos + "\n";
                    serialPort.WriteLine(cmds);
                    Console.WriteLine(cmds);
                }

                label1.Text = speed.ToString("F", CultureInfo.InvariantCulture);
                label2.Text = directionA.ToString("F", CultureInfo.InvariantCulture);
                label3.Text = directionB.ToString("F", CultureInfo.InvariantCulture);
                
                


            }

            drawData();


        }

        private void cbJoystick_TextUpdate(object sender, EventArgs e)
        {
            
        }

        private void cbJoystick_SelectedIndexChanged(object sender, EventArgs e)
        {
            Device = (GamepadDevice)cbJoystick.SelectedItem;
            acquire(this);
        }

        private void drawData()
        {
            
            while (true)
            {
                string line = "";
                lock (data_lock)
                {
                    if (data.Count > 0)
                    {
                        line = data.Dequeue();
                    }
                }
                if (line == "")
                {
                    break;
                }

                string[] parts = line.Split(' ');

                int width = img.Width;
                int height = img.Height;

                if (parts.Length == 3 && parts[0] == "S")
                {

                    int degree = Convert.ToInt32(parts[1]);
                    double f = Convert.ToDouble(parts[2].Replace("\r", ""), CultureInfo.InvariantCulture);

                    int x = Math.Min((degree * width) / 100, width - 1);
                    int y = Math.Min((((int)f * height) / 300), height - 1);

                    for (int i = 0; i < img.Height; i++)
                    {
                        img.SetPixel(x, height - i - 1, (i < y ? Color.Black : Color.White));
                    }
                    pbRadar.Image = img;

                    //Console.WriteLine(degree + " " + f);

                }
                else if (parts.Length == 2 && parts[0] == "A")
                {
                    label4.Text = parts[1];
                }
                else
                {
                    Console.WriteLine(line);
                }
            }
        }

        private void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            lock (data_lock)
            {
                if (serialPort.IsOpen)
                {
                    data.Enqueue(serialPort.ReadLine());
                }
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (serialPort.IsOpen)
            {
                serialPort.Close();
            }
        }

    }
}
