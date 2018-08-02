using System;
using System.Collections.Generic;
using System.Text;

namespace SewarDal.Models
{
    public class MapStyleJson
    {
        public string mapstyleString { get; set; }
        public MapStyleJson()
        {
            getMapStyleData();
        }
        public string getMapStyleData()
        {
            mapstyleString =
           "[\n" +
    "{\n" +
    "    \"featureType\": \"all\",\n" +
    "    \"elementType\": \"geometry.fill\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"weight\": \"2.00\"\n" +
    "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"all\",\n" +
    "    \"elementType\": \"geometry.stroke\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "           \"color\": \"#9c9c9c\"\n" +
     "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"all\",\n" +
    "    \"elementType\": \"labels.text\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"visibility\": \"on\"\n" +
    "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"landscape\",\n" +
    "    \"elementType\": \"all\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#f2f2f2\"\n" +
     "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"landscape\",\n" +
    "    \"elementType\": \"geometry.fill\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#ffffff\"\n" +
"      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"landscape.man_made\",\n" +
    "    \"elementType\": \"geometry.fill\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#ffffff\"\n" +
     "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"poi\",\n" +
    "    \"elementType\": \"all\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"visibility\": \"off\"\n" +
     "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"road\",\n" +
    "    \"elementType\": \"all\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"saturation\": -100\n" +
            "  },\n" +
            "{\n" +
    "            \"lightness\": 70\n" +
       "      },\n" +
            "{\n" +
    "            \"weight\": 0.60\n" +
       "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"road\",\n" +
    "    \"elementType\": \"geometry.fill\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#eeeeee\"\n" +
        "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"road\",\n" +
    "    \"elementType\": \"labels.text.fill\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#7b7b7b\"\n" +
          "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"road\",\n" +
    "    \"elementType\": \"labels.text.stroke\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#ffffff\"\n" +
          "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"road.highway\",\n" +
    "    \"elementType\": \"all\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"visibility\": \"simplified\"\n" +
          "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"road.arterial\",\n" +
    "    \"elementType\": \"labels.icon\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"visibility\": \"off\"\n" +
           "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"transit\",\n" +
    "    \"elementType\": \"all\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"visibility\": \"off\"\n" +
           "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"water\",\n" +
    "    \"elementType\": \"all\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#C8D7D4\"\n" +
            "  },\n" +
            "{\n" +
    "            \"visibility\": \"on\"\n" +
           "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"water\",\n" +
    "    \"elementType\": \"geometry.fill\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#C8D7D4\"\n" +
           "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"water\",\n" +
    "    \"elementType\": \"labels.text.fill\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#335d5f\"\n" +
          "      }\n" +
                "    ]\n" +
    "  },\n" +
    "{\n" +
    "    \"featureType\": \"water\",\n" +
    "    \"elementType\": \"labels.text.stroke\",\n" +
    "    \"stylers\": [\n" +
            "{\n" +
    "            \"color\": \"#335d5f\"\n" +
           "      }\n" +
                "    ]\n" +
    "      }\n" +
                "    ]\n";

            return mapstyleString;
        }



    }
}
