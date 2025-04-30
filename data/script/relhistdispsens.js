if (!window.relhistdispsensController) {
  const relhistdispsensController = {
    initialize: function () {
      const containerrelhistdispsens = document.querySelector(".js-container-relhistdispsens");

      if (!containerrelhistdispsens) {
        console.error("Elemento .js-container-relhistdispsens não encontrado.");
        return;
      }

      const formHTML = `
        <div class="form-container">
          <form id="formRelatorioHistorico">
            <span>Informe o Periodo</span><br><br>

            <label for="dataInicial">Data Inicial:</label>
            <input type="date" id="dataInicial" name="dataInicial" required><br>

            <label for="dataFinal">Data Final:</label>
            <input type="date" id="dataFinal" name="dataFinal" required><br>

            <button type="submit" id="btnGerarRelatorio">Gerar Relatório</button>
            <button type="button" id="btnExportarPDF" style="display: none;">Exportar para PDF</button>
          </form>

          <div id="resultadoRelatorio">
            <!-- O resultado do relatório será exibido aqui -->
          </div>
        </div>
      `;

      containerrelhistdispsens.innerHTML = formHTML;

      const formRelatorio = document.getElementById("formRelatorioHistorico");
      const resultadoRelatorio = document.getElementById("resultadoRelatorio");
      const btnExportarPDF = document.getElementById("btnExportarPDF");

      formRelatorio.addEventListener("submit", function (event) {
        event.preventDefault();

        const dataInicial = document.getElementById("dataInicial").value;
        const dataFinal = document.getElementById("dataFinal").value;

        if (!dataInicial || !dataFinal) {
          alert("Por favor, preencha as datas inicial e final.");
          return;
        }

        // Chamada à API para buscar os dados do relatório
        fetch(`http://192.168.1.52:8080/api.php?action=historico&dataInicial=${dataInicial}&dataFinal=${dataFinal}`)
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

            // Criar tabela para exibir os dados
            const tabelaHTML = `
              <table>
                <thead>
                  <tr>
                    <th>Data</th>
                    <th>Tipo</th>
                    <th>Nome</th>
                    <th>Valor</th>
                  </tr>
                </thead>
                <tbody>
                  ${historico.map(item => `
                    <tr>
                      <td>${item.dateregister}</td>
                      <td>${item.tipo}</td>
                      <td>${item.nome}</td>
                      <td>${item.valor}</td>
                    </tr>
                  `).join("")}
                </tbody>
              </table>
            `;

            resultadoRelatorio.innerHTML = tabelaHTML;
            btnExportarPDF.style.display = "inline-block";

            // Adicionar evento para exportar o relatório em PDF
            btnExportarPDF.onclick = function () {
              const { jsPDF } = window.jspdf;
              const doc = new jsPDF();
            
              // Configuração do título e período
              const titulo = "Relatório de Histórico de Dispositivos e Sensores";
              const periodo = `Período: ${formatarData(dataInicial)} a ${formatarData(dataFinal)}`;
            
              // Função para formatar a data no formato dd/mm/yyyy
              function formatarData(data) {
                const [ano, mes, dia] = data.split("-");
                return `${dia}/${mes}/${ano}`;
              }
            
              // Configuração da tabela
              const tabelaDados = historico.map(item => [
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
                  0: { cellWidth: 50 }, // Largura da coluna "Data/Hora"
                  1: { cellWidth: 30 }, // Largura da coluna "Tipo"
                  2: { cellWidth: 50 }, // Largura da coluna "Nome"
                  3: { cellWidth: 30 }, // Largura da coluna "Valor"
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
    }
  };

  // Certifique-se de que o controlador está acessível globalmente
  window.relhistdispsensController = relhistdispsensController;
  document.addEventListener("DOMContentLoaded", relhistdispsensController.initialize);
}