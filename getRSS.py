import requests
import xml.etree.ElementTree as ET

def fetch_aqi_data_from_xml(url):
    try:
        response = requests.get(url)
        xml_content = response.content
        tree = ET.fromstring(xml_content)

        def extract_aqi_and_timestamp(item):
            timestamp = item.find("description").text.split(";")[0]
            pm25_aqi = int(item.find("AQI").text)

            # parse timestamp like "05-07-2024 00:00"
            date_str, time_str = timestamp.split(" ")
            month, day, year = date_str.split("-")
            month, day, year = int(month), int(day), int(year)
            hour, minute = time_str.split(":")
            hour, minute = int(hour), int(minute)

            return (year, month, day, hour), pm25_aqi

        aqi_data = []

        for item in tree.findall("./channel/item"):
            timestamp, pm25_aqi = extract_aqi_and_timestamp(item)
            aqi_data.append({"timestamp": timestamp, "pm25_aqi": pm25_aqi})

        return aqi_data

    except Exception as e:
        print("Error fetching or parsing XML:", e)
        return None

def test(a):
    return a + 1

if __name__ == "__main__":
    url = "http://www.stateair.net/web/rss/1/1.xml"
    aqi_data = fetch_aqi_data_from_xml(url)
    if aqi_data:
        for data in aqi_data:
            for key, value in data.items():
                print(f"{key}: {value}")
