if (!window.relhistdispsensController) {
    const relhistdispsensController = {
      initialize: function () {
        const containerrelhistdispsens = document.querySelector(".js-container-relhistdispsens");
  
        if (!containerrelhistdispsens) {
          console.error("Elemento .js-container-relhistdispsens não encontrado.");
          return;
        }
  
        const formHTML = `
          <div class="form-container" style="display: flex; justify-content: space-between;">
            <!-- Formulário para o período -->
            <div style="width: 45%; border-radius: 5px; padding: 10px;">
              <form id="formRelatorioHistorico" style="width: 100%; height: auto; margin: 0;">
                <span style="color: white;">Informe o Período</span><br><br>
  
                <label for="dataInicial" style="color: white;">Data Inicial:</label>
                <input type="date" id="dataInicial" name="dataInicial" required><br>
  
                <label for="dataFinal" style="color: white;">Data Final:</label>
                <input type="date" id="dataFinal" name="dataFinal" required><br>
  
                <button type="submit" id="btnGerarRelatorio" style="margin-top: 10px;">Gerar Relatório</button>
              </form>
            </div>
            <div class="form-container" style="display: flex; justify-content: space-between;">
  
              <!-- Formulário para o resultado -->
              <div style="width: 100%; border-radius: 5px; padding: 10px;">
                <form id="formResultado" style="width: 100%; height: auto; margin: 0;">
                  <span style="color: white;">Resultado</span><br><br>
                  <div id="resultadoRelatorio" style="width: 100%; height: 300px; overflow-y: auto; background-color: transparent;">
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
                        <!-- Os dados da tabela serão inseridos dinamicamente aqui -->
                      </tbody>
                    </table>
                  </div>
                  <div id="navegacaoRelatorio" style="margin-top: 10px; text-align: center;">
                    <!-- Botões de navegação serão exibidos aqui -->
                  </div>
                </form>
              </div>
            </div>
          </div>
        `;
  
        containerrelhistdispsens.innerHTML = formHTML;
  
        const formRelatorio = document.getElementById("formRelatorioHistorico");
        const resultadoRelatorio = document.getElementById("resultadoRelatorio");
        const navegacaoRelatorio = document.getElementById("navegacaoRelatorio");
  
        let historicoCompleto = [];
        let paginaAtual = 1;
        const itensPorPagina = 12;
  
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
  
              historicoCompleto = historico;
              paginaAtual = 1;
              atualizarTabela();
            })
            .catch(error => {
              console.error("Erro ao buscar o relatório:", error);
              resultadoRelatorio.innerHTML = `<p>Erro ao buscar o relatório: ${error.message}</p>`;
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