// ─── Interfaz del Compilador Dummy (Grupo 5) ──────────────────
const $ = (id) => document.getElementById(id);
let ultimoResultado = null;

// ── Pestañas ──
document.querySelectorAll('.pest').forEach(p => {
  p.addEventListener('click', () => activarVista(p.dataset.vista));
});
function activarVista(vista) {
  document.querySelectorAll('.pest').forEach(p => p.classList.toggle('activa', p.dataset.vista === vista));
  document.querySelectorAll('.vista').forEach(v => v.classList.toggle('activa', v.id === 'vista-' + vista));
  if (location.hash !== '#' + vista) history.replaceState(null, '', '#' + vista);
}

// ── Editor con numeración de líneas ──
const editor = $('codigo');
const gutter = $('gutter');
function actualizarGutter() {
  const n = editor.value.split('\n').length;
  let s = '';
  for (let i = 1; i <= n; i++) s += i + '\n';
  gutter.textContent = s;
  gutter.scrollTop = editor.scrollTop;
}
editor.addEventListener('input', actualizarGutter);
editor.addEventListener('scroll', () => { gutter.scrollTop = editor.scrollTop; });

// ── Selector de ejemplos ──
async function cargarListaEjemplos() {
  try {
    const r = await fetch('/ejemplos');
    const lista = await r.json();
    const sel = $('sel-ejemplo');
    sel.innerHTML = '<option value="">— Cargar ejemplo —</option>';
    lista.forEach(e => {
      const o = document.createElement('option');
      o.value = e.archivo; o.textContent = e.nombre;
      sel.appendChild(o);
    });
  } catch (e) { /* sin conexión: se ignora */ }
}
$('sel-ejemplo').addEventListener('change', async (ev) => {
  const archivo = ev.target.value;
  if (!archivo) return;
  const r = await fetch('/ejemplo?archivo=' + encodeURIComponent(archivo));
  editor.value = await r.text();
  actualizarGutter();
  analizar();
  ev.target.value = '';
});

// ── Botones ──
$('btn-lexico').addEventListener('click',     () => { analizar('lexico'); });
$('btn-sintactico').addEventListener('click', () => { analizar('sintactico'); });
$('btn-semantico').addEventListener('click',  () => { analizar('semantico'); });
$('btn-todo').addEventListener('click',       () => { analizar(); });

async function analizar(vista) {
  const codigo = editor.value;
  if (!codigo.trim()) { setEstado('Escribe código primero.', 'err'); return; }
  setEstado('Analizando…', 'info');
  try {
    const r = await fetch('/analizar', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ codigo })
    });
    const data = await r.json();
    if (data.error) { setEstado(data.error, 'err'); return; }
    ultimoResultado = data;
    renderTodo(data);
    actualizarPipeline(data);
    resumirEstado(data);
    if (vista) activarVista(vista);
  } catch (e) {
    setEstado('Error de conexión: ' + e, 'err');
  }
}

function setEstado(msg, cls) {
  const e = $('estado');
  e.textContent = msg;
  e.className = 'estado ' + (cls || '');
}

function esc(s) {
  return String(s).replace(/[&<>]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;' }[c]));
}

// ── Categorías de tokens ──
function categoria(token) {
  if (token.startsWith('KW_')) return ['KW', 'Palabra clave'];
  if (token.startsWith('LIT_')) return ['LIT', 'Literal'];
  if (token.startsWith('OP_')) return ['OP', 'Operador'];
  if (token.startsWith('DELIM_')) return ['DELIM', 'Delimitador'];
  if (token === 'IDENTIFICADOR') return ['ID', 'Identificador'];
  if (token.startsWith('TOKEN_')) return ['ERR', 'Error léxico'];
  return ['ID', 'Otro'];
}

// ── Render: Tokens ──
function renderTokens(tokens) {
  const tb = $('tabla-tokens').querySelector('tbody');
  tb.innerHTML = '';
  $('cont-lexico').textContent = tokens.length || '';
  const conteo = {};
  tokens.forEach((t, i) => {
    const [cat, desc] = categoria(t.token);
    conteo[desc] = (conteo[desc] || 0) + 1;
    const tr = document.createElement('tr');
    if (cat === 'ERR') tr.className = 'fila-err';
    tr.innerHTML = '<td class="num">' + (i + 1) + '</td><td>' + t.linea + '</td><td>' + t.token + '</td>' +
      '<td><code>' + esc(t.lexema) + '</code></td>' +
      '<td><span class="badge cat-' + cat + '">' + desc + '</span></td>';
    tb.appendChild(tr);
  });
  // leyenda con conteos por categoría
  const ley = $('leyenda-lexico');
  if (!tokens.length) { ley.innerHTML = '<span class="resumen">No se reconocieron tokens.</span>'; return; }
  ley.innerHTML = Object.keys(conteo).map(d => {
    const cat = categoria(d === 'Palabra clave' ? 'KW_' : d === 'Literal' ? 'LIT_' :
      d === 'Operador' ? 'OP_' : d === 'Delimitador' ? 'DELIM_' :
      d === 'Error léxico' ? 'TOKEN_' : 'IDENTIFICADOR')[0];
    return '<span class="badge cat-' + cat + '">' + d + ': ' + conteo[d] + '</span>';
  }).join(' ');
}

// ── Clasificación de nodos del AST ──
function claseNodo(label) {
  if (label.startsWith('PROGRAMA')) return 'prog';
  if (label.startsWith('OP_BIN') || label.startsWith('OP_UN')) return 'op';
  if (label.startsWith('LIT_')) return 'lit';
  if (label.startsWith('ID ')) return 'id';
  if (label.startsWith('DECLARACION') || label.startsWith('ASIGNACION')) return 'decl';
  if (/^(CUANDO|LOOP|BLOQUE_SI|BLOQUE_SINO|CUERPO|CONDICION|VALOR_INICIAL|IMPRIMIR|RETORNAR)/.test(label)) return 'ctrl';
  return '';
}

// ── Render: AST (árbol anidado por profundidad) ──
function renderAST(nodos) {
  const cont = $('arbol-ast');
  cont.innerHTML = '';
  $('cont-sintactico').textContent = nodos.length || '';
  if (!nodos.length) {
    $('resumen-sintactico').textContent = 'No se generó AST.';
    cont.innerHTML = '<p class="aviso">⚠️ No hay árbol: revisa los errores sintácticos en la pestaña Semántico.</p>';
    return;
  }
  $('resumen-sintactico').textContent = nodos.length + ' nodo(s). Haz clic en un nodo con hijos para plegarlo/desplegarlo.';
  const root = document.createElement('ul');
  const stack = [{ depth: -1, ul: root }];
  nodos.forEach(n => {
    const li = document.createElement('li');
    const span = document.createElement('span');
    span.className = 'nodo ' + claseNodo(n.label);
    span.textContent = n.label;
    li.appendChild(span);
    while (stack.length > 1 && stack[stack.length - 1].depth >= n.depth) stack.pop();
    stack[stack.length - 1].ul.appendChild(li);
    const ul = document.createElement('ul');
    li.appendChild(ul);
    span.addEventListener('click', () => {
      if (ul.children.length) li.classList.toggle('colapsado');
    });
    stack.push({ depth: n.depth, ul });
  });
  // marca hojas (sin hijos) para no mostrar cursor de plegado
  cont.querySelectorAll('li').forEach(li => {
    const ul = li.querySelector('ul');
    if (ul && !ul.children.length) li.classList.add('hoja');
  });
  cont.appendChild(root);
}

// ── Render: Tabla de símbolos (agrupada por ámbito) ──
function renderSimbolos(simbolos) {
  const tb = $('tabla-simbolos').querySelector('tbody');
  tb.innerHTML = '';
  if (!simbolos.length) {
    tb.innerHTML = '<tr><td colspan="5" class="vacio">Sin símbolos (programa vacío o error sintáctico).</td></tr>';
    return;
  }
  simbolos.forEach(s => {
    const tr = document.createElement('tr');
    const sc = parseInt(s.scope, 10) || 0;
    tr.innerHTML = '<td><code>' + esc(s.nombre) + '</code></td>' +
      '<td><span class="tipo tipo-' + s.tipo + '">' + s.tipo + '</span></td>' +
      '<td><span class="scope scope-' + (sc % 6) + '">' + (sc === 0 ? 'global (0)' : sc) + '</span></td>' +
      '<td>' + s.linea + '</td>' +
      '<td>' + (s.init === 'si' ? '✅ sí' : '⬜ no') + '</td>';
    tb.appendChild(tr);
  });
}

// ── Render: Diagnósticos (errores léxicos/sintácticos/semánticos) ──
function renderErrores(data) {
  const ul = $('lista-errores');
  ul.innerHTML = '';
  const errs = (data.errores || []).slice();
  // errores léxicos: tokens TOKEN_ERROR
  (data.tokens || []).filter(t => t.token.startsWith('TOKEN_')).forEach(t => {
    errs.unshift({ tipo: 'lexico', linea: t.linea, mensaje: "Carácter no reconocido: '" + t.lexema + "'" });
  });

  $('cont-semantico').textContent = errs.length || '';

  if (!errs.length) {
    const li = document.createElement('li');
    li.className = 'ok';
    li.textContent = data.sintactico_ok
      ? '✅ Análisis completado sin errores (léxico, sintáctico y semántico correctos).'
      : '✅ Sin diagnósticos.';
    ul.appendChild(li);
    return;
  }
  const etq = { lexico: 'Léxico', sintactico: 'Sintáctico', semantico: 'Semántico' };
  errs.sort((a, b) => a.linea - b.linea);
  errs.forEach(e => {
    const li = document.createElement('li');
    li.className = 'err-' + e.tipo;
    li.innerHTML = '<span class="tag tag-' + e.tipo + '">' + (etq[e.tipo] || e.tipo) + '</span>' +
      '<span class="lin">Línea ' + e.linea + '</span> ' + esc(e.mensaje);
    ul.appendChild(li);
  });
}

function renderTodo(data) {
  renderTokens(data.tokens || []);
  renderAST(data.ast || []);
  renderSimbolos(data.simbolos || []);
  renderErrores(data);
}

// ── Barra de pipeline (① Léxico → ② Sintáctico → ③ Semántico) ──
function marcarFase(fase, estado, texto) {
  const el = document.querySelector('.fase[data-fase="' + fase + '"]');
  if (!el) return;
  el.className = 'fase ' + estado;
  el.querySelector('b').textContent = texto;
}
function actualizarPipeline(data) {
  const hayErrLex = (data.tokens || []).some(t => t.token.startsWith('TOKEN_'));
  marcarFase('lexico', hayErrLex ? 'err' : 'ok',
    hayErrLex ? '✖' : (data.tokens.length + ' tk'));
  marcarFase('sintactico', data.sintactico_ok ? 'ok' : 'err',
    data.sintactico_ok ? '✔' : '✖');
  const est = data.semantico_estado;
  marcarFase('semantico', est === 'ok' ? 'ok' : est === 'na' ? 'na' : 'err',
    est === 'ok' ? '✔' : est === 'na' ? '—' : '✖');
}

function resumirEstado(data) {
  const nErr = (data.errores || []).length;
  let parts = [];
  parts.push('Léxico: ' + data.tokens.length + ' tokens');
  parts.push(data.sintactico_ok ? 'Sintáctico: ✅' : 'Sintáctico: ❌');
  parts.push(data.semantico_estado === 'ok' ? 'Semántico: ✅'
    : data.semantico_estado === 'na' ? 'Semántico: —'
    : 'Semántico: ❌ ' + nErr + ' error(es)');
  setEstado(parts.join('   ·   '),
    data.sintactico_ok && data.semantico_estado === 'ok' ? 'ok' : 'err');
}

// ── Inicio: cargar lista y ejemplo por defecto ──
window.addEventListener('load', async () => {
  await cargarListaEjemplos();
  const params = new URLSearchParams(location.search);
  const archivo = params.get('archivo') || 'ejemplo1.g5z80';
  const r = await fetch('/ejemplo?archivo=' + encodeURIComponent(archivo));
  editor.value = await r.text();
  actualizarGutter();
  await analizar();
  const h = location.hash.replace('#', '');
  if (['lexico', 'sintactico', 'semantico'].includes(h)) activarVista(h);
});
