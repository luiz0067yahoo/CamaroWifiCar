package CamaroWifiCar;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.client.RestTemplate;

@Controller
public class CarrinhoController {

    @Autowired
    private RestTemplate restTemplate;

    // Rota para exibir a página de controle
    @GetMapping("/controle")
    public String exibirControle(Model model) {
        return "controle"; // Nome do template HTML (controle.html)
    }

    // Rota para lidar com comandos enviados pelo usuário
    @PostMapping("/comando")
    public String enviarComando(@RequestParam("acao") String acao, Model model) {
        // Processar o comando recebido
        System.out.println("Comando recebido: " + acao);

        // Enviar o comando para o ESP32
        String esp32Url = "http://192.168.137.250/api/comando"; // Substitua pelo IP do seu ESP32
        String jsonDados = "{\"acao\":\"" + acao + "\"}";

        ResponseEntity<String> response = restTemplate.postForEntity(esp32Url, jsonDados, String.class);

        // Verificando a resposta do ESP32
        if (response.getStatusCode().is2xxSuccessful()) {
            System.out.println("Comando enviado ao ESP32: " + response.getBody());
        } else {
            System.out.println("Erro ao enviar comando ao ESP32: " + response.getStatusCode());
        }

        // Retorna a página de controle com uma mensagem de confirmação
        model.addAttribute("mensagem", "Comando " + acao + " enviado com sucesso!");
        return "controle";
    }
}