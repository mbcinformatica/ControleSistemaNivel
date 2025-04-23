if (!window.dispositivosController) {
  const dispositivosController = {
    initialize: function () {
      var inicializado = false;
      const containerDispositivos = document.querySelector(".js-container-dispositivos");

      function renderDispositivos(dispositivos) {
        containerDispositivos.innerHTML = "";
        const listaDispositivos = document.createElement("ul");

        dispositivos.forEach(function (dispositivo) {
          if (dispositivo.identificador != "botao_reset") {

            const itemDispositivo = document.createElement("li");
            itemDispositivo.id = `dispositivo-${dispositivo.pino}`;

            const spanNome = document.createElement("span");
            spanNome.textContent = dispositivo.nome;
            itemDispositivo.appendChild(spanNome);

            const divLamp = document.createElement("div");
            divLamp.classList.add("lamp");
            if (dispositivo.identificador === "alarme_sistema") {
              divLamp.style.position = "absolute"; // Adicionado
            } else {
              divLamp.style.position = "relative";
            }

            const imgLamp = document.createElement("img");
            imgLamp.src = dispositivo.estado ? dispositivo.imageon : dispositivo.imageoff;
            imgLamp.width = dispositivo.largura;
            imgLamp.height = dispositivo.altura;
            imgLamp.className = "imgDispositivo";
            imgLamp.style.padding = dispositivo.preenchimento;
            divLamp.appendChild(imgLamp);

            if (dispositivo.mostrabotao === true) {
              const imgBotao = document.createElement("img");
              imgBotao.src = dispositivo.estado ? "img/Botao_Desliga.png" : "img/Botao_Liga.png";
              imgBotao.width = 30;
              imgBotao.height = 30;
              imgBotao.className = "btnDispositivo";
              imgBotao.style.position = "absolute"; // Adicionado
              imgBotao.style.bottom = "4px"; // Ajuste conforme necessário
              imgBotao.style.left = "32%"; // Centralizado
              imgBotao.style.transform = "translateX(-50%)"; // Centralizado

              imgBotao.addEventListener("click", function () {
                enviarComando(dispositivo.pino);
              });
              divLamp.appendChild(imgBotao);
            }
            itemDispositivo.appendChild(divLamp);

            if (dispositivo.identificador === "bomba_entrada" || dispositivo.identificador === "bomba_saida") {

              const imgNivel = document.createElement("img");
              if (dispositivo.identificador === "bomba_entrada") {
                imgNivel.src = dispositivo.estado ? "img/CaixaEnchendo.gif" : "img/SemImagemNivel.gif";
              } else if (dispositivo.identificador === "bomba_saida") {
                imgNivel.src = dispositivo.estado ? "img/CaixaEsvaziando.gif" : "img/SemImagemNivel.gif";
              }
              imgNivel.width = 130;
              imgNivel.height = 130;
              imgNivel.className = "imgNivelDispositivo";
              imgNivel.style.padding = "0px 10px";
              itemDispositivo.appendChild(imgNivel);
            }
            if (dispositivo.identificador === "alarme_sistema") {

              const imgReset = document.createElement("img");
              imgReset.src = "img/Botao_Reset.png"
              imgReset.width = 50;
              imgReset.height = 50;
              imgReset.className = "imgResetDispositivo";
              imgReset.style.padding = "50px 176px";
              imgReset.addEventListener("click", function () {
                enviarComando(2);

              });
              itemDispositivo.appendChild(imgReset);
            }
            listaDispositivos.appendChild(itemDispositivo);
          }
        });
        containerDispositivos.appendChild(listaDispositivos);
      }

      function atualizarDispositivosHTML(dispositivos) {
        dispositivos.forEach(dispositivo => {
          const itemDispositivo = document.querySelector(`#dispositivo-${dispositivo.pino}`);
          if (!itemDispositivo) return;

          const imgDispositivo = itemDispositivo.querySelector(".imgDispositivo");
          imgDispositivo.src = dispositivo.estado ? dispositivo.imageon : dispositivo.imageoff;
          if (dispositivo.mostrabotao === 1) {
            const btnDispositivo = itemDispositivo.querySelector(".btnDispositivo");
            btnDispositivo.src = dispositivo.estado ? "img/Botao_Desliga.png" : "img/Botao_Liga.png";
          }

          if (dispositivo.identificador === "bomba_entrada" || dispositivo.identificador === "bomba_saida") {
            const imgNivelDispositivo = itemDispositivo.querySelector(".imgNivelDispositivo");
            if (dispositivo.identificador === "bomba_entrada") {
              imgNivelDispositivo.src = dispositivo.estado ? "img/CaixaEnchendo.gif" : "img/SemImagemNivel.gif";
            } else if (dispositivo.identificador === "bomba_saida") {
              imgNivelDispositivo.src = dispositivo.estado ? "img/CaixaEsvaziando.gif" : "img/SemImagemNivel.gif";
            }
          }
        });
      }

      function atualizarDispositivos() {
        fetch("/api/dispositivos")
          .then((response) => response.json())
          .then((data) => {
            let dispositivos = data.dispositivos;
            if (inicializado) {
              atualizarDispositivosHTML(dispositivos);
              console.log(dispositivos);              
            } else {
              renderDispositivos(dispositivos);
              inicializado = true;
            }
          })
          .catch((error) => console.error("Erro ao buscar dispositivos:", error));
      }

      function enviarComando(pino) {
        fetch('/api/comando', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            pino: pino,
          })
        })
          .then(response => response.json())
          .then(() => atualizarDispositivos())
          .catch(error => console.error('Erro ao enviar comando:', error));
      }

      setInterval(atualizarDispositivos, 1500);
      atualizarDispositivos();
    }
  };

  // Certifique-se de que o controlador está acessível globalmente
  window.dispositivosController = dispositivosController;
}
