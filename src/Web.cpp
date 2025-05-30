#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include "credentials.h"
#include "Web.h"

Preferences preferences;
WebServer server(80);

String ssid = "", password = "";

void setupAP()
{
    WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASSWORD);
    Serial.println("Access Point iniciado");
    Serial.println(WiFi.softAPIP());
}

bool loadWiFiCredentials()
{
    preferences.begin("wifi", true);
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    preferences.end();
    return ssid != "" && password != "";
}

void connectToWiFi()
{
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.printf("Conectando a %s", ssid.c_str());
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
}

bool isAuthenticated()
{
    return server.authenticate(WEB_USERNAME, WEB_PASSWORD);
}

String getFileListHTML()
{
    String html = "";
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file)
    {
        String filename = file.name();

        if (filename.endsWith(".ch8"))
        {
            html += "<li>" + filename + " (" + String(file.size()) + " bytes)";
            html += " <button onclick=\"deleteFile('" + filename + "')\">Excluir</button></li>";
        }
        file = root.openNextFile();
    }
    return html;
}

void handleIndex()
{
    if (!isAuthenticated())
        return server.requestAuthentication();
    File file = SPIFFS.open("/index.html", "r");
    if (!file)
    {
        server.send(500, "text/plain", "index.html não encontrado");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

void handleUpload()
{
    HTTPUpload &upload = server.upload();
    static File f;
    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = "/" + upload.filename;
        if (!filename.endsWith(".ch8"))
        {
            server.send(400, "text/plain", "Somente arquivos .ch8 são permitidos");
            return;
        }
        f = SPIFFS.open(filename, FILE_WRITE);
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (f)
            f.write(upload.buf, upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (f)
            f.close();
        server.send(200, "text/plain", "OK");
    }
}

void handleDelete()
{
    if (!isAuthenticated())
        return server.requestAuthentication();
    String filename = server.arg("file");
    if (SPIFFS.exists("/" + filename) && filename.endsWith(".ch8"))
    {
        SPIFFS.remove("/" + filename);
        server.send(200, "text/plain", "Arquivo deletado");
    }
    else
    {
        server.send(404, "text/plain", "Arquivo não encontrado");
    }
}

void handleLogout() {
  server.send(200, "text/html", "<h3>Você saiu. Reiniciando...</h3>");
  delay(2000);
  ESP.restart();
}

void handleListFiles()
{
    if (!isAuthenticated())
        return server.requestAuthentication();
    server.send(200, "text/html", getFileListHTML());
}

void setupWebServer()
{
    server.on("/", handleIndex);
    server.on("/index.html", handleIndex);
    server.on("/upload", HTTP_POST, []() {}, handleUpload);
    server.on("/delete", handleDelete);
    server.on("/list", handleListFiles);
    server.on("/logout", handleLogout);
    server.begin();
}

void setupWeb()
{
    if (!loadWiFiCredentials())
    {
        setupAP();
    }
    else
    {
        connectToWiFi();
        if (WiFi.status() != WL_CONNECTED)
        {
            setupAP();
        }
    }

    setupWebServer();
    Serial.println("Servidor HTTP iniciado");
}

void loopWeb()
{
    server.handleClient();
}
