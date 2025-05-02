if (!window.relhistdispsensController) {
  const relhistdispsensController = {
    initialize: function () {
      const containerrelhistdispsens = document.querySelector(".js-container-relhistdispsens");

      const formHTML = `
        <div class="form-container">
          <div style="width: 25%; height: 100%; border-radius: 5px; margin-left: 8px;">
            <form id="formRelatorioHistorico" style="width: auto;height: 100%;">
              <span style="color: white;">Informe o Período</span><br><br>
              <label for="dataInicial" style="color: white;">Data Inicial:</label>
              <input type="date" id="dataInicial" name="dataInicial" required><br>
              <label for="horaInicial" style="color: white;">Hora Inicial:</label>
              <input type="time" id="horaInicial" name="horaInicial" required><br>
              <label for="dataFinal" style="color: white;">Data Final:</label>
              <input type="date" id="dataFinal" name="dataFinal" required><br>
              <label for="horaFinal" style="color: white;">Hora Final:</label>
              <input type="time" id="horaFinal" name="horaFinal" required><br>
              <button type="submit" id="btnGerarRelatorio" style="margin-top: 10px;">Gerar Relatório</button>
              <button type="button" id="btnExportarPDF" style="margin-top: 10px; display: none;">Exportar para PDF</button>
            </form>
          </div>
          <div style="width: 75%; height: 100%; border-radius: 5px; margin-left: 8px;">
            <form id="formResultado" style="width: auto; height: auto;">
              <span style="color: white;">Resultado</span><br><br>
              <div id="resultadoRelatorio" style="width: 100%;height: auto;overflow-y: auto;background-color: transparent;">
                <table style="width: 100%;border-collapse: collapse;text-align: left;color: #ffffff;">
                  <thead>
                    <tr style="background-color: #003366; color: white;">
                      <th style="width: 9%;padding: 8px;border: 1px solid white;">Data</th>
                      <th style="width: 3%;padding: 8px;border: 1px solid white;">Tipo</th>
                      <th style="width: 26%;padding: 8px;border: 1px solid white;">Nome</th>
                      <th style="width: 8%;padding: 8px;border: 1px solid white;">Valor</th>
                    </tr>
                  </thead>
                </table>
              </div>
              <div id="navegacaoRelatorio" style="margin-top: 10px; text-align: center;"></div>
            </form>
          </div>
        </div>
      `;

      containerrelhistdispsens.innerHTML = formHTML;

      const formRelatorio = document.getElementById("formRelatorioHistorico");
      const resultadoRelatorio = document.getElementById("resultadoRelatorio");
      const navegacaoRelatorio = document.getElementById("navegacaoRelatorio");
      const btnExportarPDF = document.getElementById("btnExportarPDF");

      let historicoCompleto = [];
      let paginaAtual = 1;
      const itensPorPagina = 12;

      formRelatorio.addEventListener("submit", function (event) {
        event.preventDefault();

        const dataInicial = document.getElementById("dataInicial").value;
        const horaInicial = document.getElementById("horaInicial").value;
        const dataFinal = document.getElementById("dataFinal").value;
        const horaFinal = document.getElementById("horaFinal").value;

        if (!dataInicial || !horaInicial || !dataFinal || !horaFinal) {
          alert("Por favor, preencha todos os campos de data e hora.");
          return;
        }

        // Combina data e hora para formar os parâmetros completos
        const dataHoraInicial = `${dataInicial} ${horaInicial}`;
        const dataHoraFinal = `${dataFinal} ${horaFinal}`;

        // Chamada à API para buscar os dados do relatório
        fetch(`http://192.168.1.52:8080/api.php?action=historico&dataInicial=${dataHoraInicial}&dataFinal=${dataHoraFinal}`)
          .then(response => {
            if (!response.ok) {
              throw new Error(`Erro ao buscar o relatório: ${response.statusText}`);
            }
            return response.json();
          })
          .then(historico => {
            if (!Array.isArray(historico)) {
              throw new Error("Resposta inválida da API. Esperado um array.");
            }

            // Limpar o resultado anterior
            resultadoRelatorio.innerHTML = "";

            if (historico.length === 0) {
              resultadoRelatorio.innerHTML = "<p>Nenhum registro encontrado para o período informado.</p>";
              btnExportarPDF.style.display = "none";
              return;
            }

            historicoCompleto = historico;
            paginaAtual = 1;
            atualizarTabela();
            btnExportarPDF.style.display = "inline-block";

            // Adicionar evento para exportar o relatório em PDF
            btnExportarPDF.onclick = function () {
              const { jsPDF } = window.jspdf;
              const doc = new jsPDF({ orientation: "landscape" }); // A4 em paisagem

              // Configuração do título e período
              const titulo = "Relatório de Histórico de Dispositivos e Sensores";
              const periodo = `Período: ${dataHoraInicial} a ${dataHoraFinal}`;

              // Função para formatar a data no formato dd/mm/yyyy
              function formatarData(data) {
                const [ano, mes, dia] = data.split("-");
                return `${dia}/${mes}/${ano}`;
              }

              // Configuração da tabela
              const tabelaDados = historicoCompleto.map(item => [
                formatarData(item.dateregister.split(" ")[0]) + " " + item.dateregister.split(" ")[1], // Data/Hora formatada
                item.tipo,         // Tipo
                item.nome,         // Nome
                item.valor         // Valor
              ]);

              // Cabeçalhos da tabela
              const tabelaCabecalho = ["Data/Hora", "Tipo", "Nome", "Valor"];

              // Adiciona a tabela ao PDF usando autoTable
              doc.autoTable({
                head: [tabelaCabecalho],
                body: tabelaDados,
                startY: 30, // Posição inicial da tabela
                styles: {
                  fontSize: 10, // Tamanho da fonte
                  cellPadding: 3, // Espaçamento interno das células
                },
                headStyles: {
                  fillColor: [200, 200, 200], // Cor de fundo do cabeçalho
                  textColor: 0, // Cor do texto do cabeçalho
                  fontStyle: "bold", // Estilo do texto do cabeçalho
                },
                bodyStyles: {
                  textColor: 0, // Cor do texto das células
                },
                alternateRowStyles: {
                  fillColor: [240, 240, 240], // Cor de fundo alternada para as linhas
                },
                columnStyles: {
                  0: { cellWidth: 70 }, // Largura da coluna "Data/Hora"
                  1: { cellWidth: 40 }, // Largura da coluna "Tipo"
                  2: { cellWidth: 100 }, // Largura da coluna "Nome"
                  3: { cellWidth: 40 }, // Largura da coluna "Valor"
                },
                didDrawPage: function (data) {
                  // Adiciona o título no topo de cada página
                  doc.setFontSize(14);
                  doc.text(titulo, 10, 10);

                  // Adiciona o período no topo de cada página
                  doc.setFontSize(12);
                  doc.text(periodo, 10, 20);
                },
                margin: { top: 30 }, // Ajusta a margem superior para o cabeçalho
              });

              // Salva o PDF com o nome especificado
              doc.save("relatorio_historico.pdf");
            };
          })
          .catch(error => {
            console.error("Erro ao buscar o relatório:", error);
            resultadoRelatorio.innerHTML = `<p>Erro ao buscar o relatório: ${error.message}</p>`;
            btnExportarPDF.style.display = "none";
          });
      });

      function atualizarTabela() {
        const inicio = (paginaAtual - 1) * itensPorPagina;
        const fim = inicio + itensPorPagina;
        const historicoPagina = historicoCompleto.slice(inicio, fim);

        // Limpar o resultado anterior
        resultadoRelatorio.innerHTML = "";

        if (historicoPagina.length === 0) {
          resultadoRelatorio.innerHTML = "<p>Nenhum registro encontrado para o período informado.</p>";
          navegacaoRelatorio.innerHTML = "";
          return;
        }

        // Criar tabela para exibir os dados
        const tabelaHTML = `
          <table style="width: 100%; border-collapse: collapse; text-align: left; color: white; background-color: transparent;">
            <thead>
              <tr style="background-color: #003366; color: white;">
                <th style="padding: 8px; border: 1px solid white;">Data</th>
                <th style="padding: 8px; border: 1px solid white;">Tipo</th>
                <th style="padding: 8px; border: 1px solid white;">Nome</th>
                <th style="padding: 8px; border: 1px solid white;">Valor</th>
              </tr>
            </thead>
            <tbody>
              ${historicoPagina.map(item => `
                <tr>
                  <td style="padding: 8px; border: 1px solid white;">${item.dateregister}</td>
                  <td style="padding: 8px; border: 1px solid white;">${item.tipo}</td>
                  <td style="padding: 8px; border: 1px solid white;">${item.nome}</td>
                  <td style="padding: 8px; border: 1px solid white;">${item.valor}</td>
                </tr>
              `).join("")}
            </tbody>
          </table>
        `;

        resultadoRelatorio.innerHTML = tabelaHTML;

        atualizarNavegacao();
      }

      function atualizarNavegacao() {
        const totalPaginas = Math.ceil(historicoCompleto.length / itensPorPagina);
        navegacaoRelatorio.innerHTML = "";
      
        if (totalPaginas <= 1) return;
      
        // Exibir o contador de página
        const contadorPagina = document.createElement("span");
        contadorPagina.textContent = `Página ${paginaAtual}/${totalPaginas}`;
        contadorPagina.style.marginRight = "20px";
        contadorPagina.style.color = "white";
        contadorPagina.style.fontWeight = "bold";
        navegacaoRelatorio.appendChild(contadorPagina);
      
        // Botão "Anterior"
        if (paginaAtual > 1) {
          const btnAnterior = document.createElement("button");
          btnAnterior.textContent = "Anterior";
          btnAnterior.style.marginRight = "10px";
          btnAnterior.onclick = () => {
            paginaAtual--;
            atualizarTabela();
          };
          navegacaoRelatorio.appendChild(btnAnterior);
        }
      
        // Botão "Próximo"
        if (paginaAtual < totalPaginas) {
          const btnProximo = document.createElement("button");
          btnProximo.textContent = "Próximo";
          btnProximo.onclick = () => {
            paginaAtual++;
            atualizarTabela();
          };
          navegacaoRelatorio.appendChild(btnProximo);
        }
      }
      
    }
  };

  // Certifique-se de que o controlador está acessível globalmente
  window.relhistdispsensController = relhistdispsensController;
  document.addEventListener("DOMContentLoaded", relhistdispsensController.initialize);
}