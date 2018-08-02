using Plugin.Geolocator;
using Plugin.Permissions;
using Plugin.Permissions.Abstractions;
using SewarDal.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.GoogleMaps;
using Xamarin.Forms.Xaml;

namespace SewarDal
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class MapPage : ContentPage
	{
		public MapPage ()
		{
			InitializeComponent ();

            NavigationPage.SetHasBackButton(this, false);
            whiteBarBottom.Source = ImageSource.FromResource("SewarDal.Images.WhiteBarOpacity200.png");
            LocateMeButton.Source = ImageSource.FromResource("SewarDal.Images.locateMeButton.png");
            MainMap.MoveToRegion(MapSpan.FromCenterAndRadius(new Position(21.4874338, 39.2174196), Distance.FromMiles(8)), true);
            MapStyleJson mymapstyle = new MapStyleJson();
            MainMap.MapStyle = MapStyle.FromJson(mymapstyle.mapstyleString);
            GetLocationPermision();
        }

        private async void GetLocationPermision()
        {

            try
            {
                var status = await CrossPermissions.Current.CheckPermissionStatusAsync(Permission.Location);
                if (status != PermissionStatus.Granted)
                {
                    if (await CrossPermissions.Current.ShouldShowRequestPermissionRationaleAsync(Permission.Location))
                    {
                        await DisplayAlert("Need location", "Gunna need that location", "OK");
                    }

                    var results = await CrossPermissions.Current.RequestPermissionsAsync(Permission.Location);
                    //Best practice to always check that the key exists
                    if (results.ContainsKey(Permission.Location))
                        status = results[Permission.Location];
                }

                if (status == PermissionStatus.Granted)
                {
                    var userPosition = await CrossGeolocator.Current.GetPositionAsync();
                    //LabelGeolocation.Text = "Lat: " + results.Latitude + " Long: " + results.Longitude;
                    MainMap.MoveToRegion(MapSpan.FromCenterAndRadius(new Position(userPosition.Latitude, userPosition.Longitude), Distance.FromMiles(1)), true);

                }
                else if (status != PermissionStatus.Unknown)
                {
                    await DisplayAlert("Location Denied", "Can not continue, try again.", "OK");
                }
            }
            catch (Exception ex)
            {

                //LabelGeolocation.Text = "Error: " + ex;
            }
        }

        private void ToolbarItem_SettingsClicked(object sender, EventArgs e)
        {

        }

        private async void TapGestureRecognizer_Tapped(object sender, EventArgs e)
        {
            await Navigation.PushAsync(new BluetoothList());
        }
    }
}