#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

const char* accessPointName = "Portal de Cativa AP"; 
const char* accessPointPassword = "12345678";        

DNSServer dnsServer;
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column; /* Alinha os containers verticalmente */
            height: 100vh;
            margin: 0;
        }
        .container {
            background: white;
            padding: 40px;
            border-radius: 5px;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
            margin-bottom: 10px; /* Espaço de 10px entre os containers */
        }
        h2 {
            text-align: justify;
        }
        .input-group {
            margin: 10px 0;
            margin-left: -10px;
        }
        label {
            display: block;
            margin-bottom: 5px;
        }
        input {
            width: 100%;
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        button {
            width: 100%;
            padding: 10px;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin-left: 5px;
        }
        button:hover {
            background-color: #0056b3;
        }
        #message {
            text-align: center;
            margin-top: 10px;
            color: red;
        }
    </style>
    <title>Login</title>
</head>
<body>
    <div class="container">
        <h2>Login</h2>
        <form id="loginForm">
            <div class="input-group">
                <label for="username">Usuário:</label>
                <input type="text" id="username" required>
            </div>
            <div class="input-group">
                <label for="password">Senha:</label>
                <input type="password" id="password" required>
            </div>
            <button type="submit">Entrar</button>
        </form>
        <div id="message"></div>
    </div>
    <div class="container">
        <p>O login é obrigatório para conexão nessa rede.</p>
    </div>
    <script>
        document.getElementById('loginForm').addEventListener('submit', function(event) {
            event.preventDefault(); // Evita o envio do formulário

            const username = document.getElementById('username').value;
            const password = document.getElementById('password').value;

            // Validação simples (substitua pela lógica real)
            if (username === 'admin' && password === 'password') {
                document.getElementById('message').textContent = 'Login bem-sucedido!';
                document.getElementById('message').style.color = 'green';
            } else {
                document.getElementById('message').textContent = 'Usuário ou senha incorretos.';
            }
        });
    </script>
</body>
</html>
)rawliteral";

class CaptivePortalHandler : public AsyncWebHandler {
public:
  CaptivePortalHandler() {}
  virtual ~CaptivePortalHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    return request->url() == "/";
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
  }
};

void setup() {
  Serial.begin(115200);
  delay(10);

  
  WiFi.softAP(accessPointName, accessPointPassword);
  Serial.print("Ponto de Acesso IP: ");
  Serial.println(WiFi.softAPIP());

  
  dnsServer.start(53, "*", WiFi.softAPIP());

  
  server.addHandler(new CaptivePortalHandler()).setFilter(ON_AP_FILTER);
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
  });

  server.begin(); 
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  dnsServer.processNextRequest(); 
}
