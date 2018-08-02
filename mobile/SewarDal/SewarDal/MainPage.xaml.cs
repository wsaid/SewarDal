using Plugin.Permissions;
using Plugin.Permissions.Abstractions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace SewarDal
{
	public partial class MainPage : ContentPage
	{
		public MainPage()
		{
			InitializeComponent();
            NavigationPage.SetHasNavigationBar(this, false);
            //SplashBG.Source = ImageSource.FromResource("SewarDal.Images.splashBG.png");
            mainLogo.Source = ImageSource.FromResource("SewarDal.Images.dallogoS.png");


            GrantPermissions();
        }

        private async void GrantPermissions()
        {
            await Task.Delay(3000);
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
                    //var userPosition = await CrossGeolocator.Current.GetPositionAsync();
                    //LabelGeolocation.Text = "Lat: " + results.Latitude + " Long: " + results.Longitude;
                    //MainMap.MoveToRegion(MapSpan.FromCenterAndRadius(new Position(userPosition.Latitude, userPosition.Longitude), Distance.FromMiles(1)), true);
                    goToMapPage();
                }
                else if (status != PermissionStatus.Unknown)
                {
                    await DisplayAlert("Location Denied", "Can not continue, try again.", "OK");
                    
                }
            }
            catch (Exception ex)
            {

                
            }
        }

        async void goToMapPage()
        {
            
            await Navigation.PushAsync(new MapPage());
            
            //Application.Current.MainPage = new MapPage();
            //await Navigation.PopToRootAsync(true);
        }


    }
}
