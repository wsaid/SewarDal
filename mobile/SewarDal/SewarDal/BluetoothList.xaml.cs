using MvvmCross.Platform;
using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;
using System;
using System.Collections.Generic;
using System.Linq;

using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace SewarDal
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class BluetoothList : ContentPage
	{
        public BluetoothList ()
		{
			InitializeComponent ();

            startScanning();
        }

        private void startScanning()
        {
            this.Appearing += MainPage_Appearing;
        }
        private async void MainPage_Appearing(object sender, EventArgs e)
        {
            var ble = CrossBluetoothLE.Current;
            var adapter = CrossBluetoothLE.Current.Adapter;


            bluetoothStatus.Text = ble.State.ToString();

            //adapter.DeviceDiscovered += (s, a) => deviceList.Add(a.Device);
            await adapter.StartScanningForDevicesAsync();

        }

    }
}