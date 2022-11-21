#include "menu_groceries.h"

static char * main_groceries[MAX_GROCERIES_ITEM + 1];;

static char * bai_groceries[MAX_GROCERIES_ITEM + 1];

static void
parse_csv_response(const char * response, char ** table) {
  char * buffer = strdup(response);

  if (buffer == NULL)
    return;

  const char * tok;
  uint16_t count = 0;

  for (tok = strtok(buffer, "\n");
       tok && *tok;
       tok = strtok(NULL, "\n")
    ) {
    Serial.printf("orig. line: %s\n", tok);
    if (count != 0) { // skipping header
      if (table[count] != NULL)
        free(table[count]);

      char * line = strdup(tok);

      Serial.printf("line: %s\n", line);

      if (line == NULL)
        return;

      const char * value;
      value = strtok(line, ",");
      Serial.printf("value: %s\n", value);

      free(line);
    }
    count++;
  }

  free(buffer);
}

static bool
fetch_groceries(void) {
  bool status = false;

  if (connectWiFi()) {
    HTTPClient http;

    http.begin(URL_GROCERIES_MAIN);

    int http_res_code = http.GET();

    if (http_res_code == 200) {
      String response = http.getString();

      Serial.printf("Response: %s\n", response.c_str());

      parse_csv_response(response.c_str(), main_groceries);

      status = true;
    }

    //turn off radios
    WiFi.mode(WIFI_OFF);
    // Are we sure about that ? Isn't btStop fow BT ?
    btStop();
  }

  return status;
}

void
menu_groceries(void) {
  display.setFullWindow();
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.setCursor(50, 80);

  if (fetch_groceries() == true)
    display.println("Success!");
  else
    display.println("Failure!");

  display.display(false); //full refresh

  show_menu(menu_index, false);
}
