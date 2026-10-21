// KitNote website: language, the step grid that plays, the pattern library, the recording demo, the AI terminal and
// the download links. The patterns are KitNote's own (src/notation/internal/notationinteraction.cpp, drumPatternData).
(() => {
  'use strict';
  const $ = (s, el = document) => el.querySelector(s);
  const $$ = (s, el = document) => [...el.querySelectorAll(s)];
  const REPO = 'hs-hui/KitNote';
  const reduced = matchMedia('(prefers-reduced-motion: reduce)').matches;

  // ------------------------------------------------------------------ language
  let lang = '';
  try { lang = localStorage.getItem('kitnote-lang') || ''; } catch (e) { /* storage off */ }
  if (lang !== 'en' && lang !== 'ko') lang = (navigator.language || '').toLowerCase().startsWith('ko') ? 'ko' : 'en';
  const T = (en, ko) => (lang === 'ko' ? ko : en);
  const onLang = [];
  function applyLang() {
    document.documentElement.lang = lang;
    for (const el of $$('[data-ko]')) {
      if (el.dataset.en === undefined) el.dataset.en = el.innerHTML;
      el.innerHTML = lang === 'ko' ? el.dataset.ko : el.dataset.en;
    }
    const btn = $('#lang');
    btn.textContent = lang === 'ko' ? 'English' : '한국어';
    btn.lang = lang === 'ko' ? 'en' : 'ko';
    $('#guide').href = `https://github.com/${REPO}/blob/main/docs/kitnote/user-guide.${lang}.md`;
    document.title = T('KitNote — drum notation, nothing else', 'KitNote — 드럼 악보 편집기');
    onLang.forEach((f) => f());
  }
  $('#lang').addEventListener('click', () => {
    lang = lang === 'ko' ? 'en' : 'ko';
    try { localStorage.setItem('kitnote-lang', lang); } catch (e) { /* storage off */ }
    applyLang();
  });

  // ------------------------------------------------------------------ the section tabs follow the page
  // In the library, the tab of the open panel is the one lit.
  const tabLinks = $$('.tabs a');
  let inView = 'grid';
  function spyMark() {
    const target = inView === 'patterns' ? (['audio', 'score', 'ai'].includes(cat) ? cat : 'patterns') : inView;
    tabLinks.forEach((a) => a.classList.toggle('on', a.hash === `#${target}`));
  }
  const spy = new IntersectionObserver((entries) => {
    for (const e of entries) {
      if (!e.isIntersecting) continue;
      inView = e.target.id;
      spyMark();
    }
  }, { rootMargin: '-45% 0px -50% 0px' });
  ['grid', 'patterns', 'download'].forEach((id) => spy.observe(document.getElementById(id)));

  // ------------------------------------------------------------------ the drums: KitNote's own sounds
  // assets/drums/kit.mp3 holds one stroke of every drum, played by KitNote with its drum SoundFont (the
  // percussion of MS Basic, MIT licence: assets/drums/LICENSE-MS-Basic.md): plain, accented (_a) and ghost (_g).
  // assets/drums/kit.json says where each stroke starts and how long it rings, in seconds.
  let ac = null, out = null, kit = null, kitLoading = null, openHat = null;
  const kitFiles = Promise.all([
    fetch('assets/drums/kit.mp3').then((r) => r.arrayBuffer()),
    fetch('assets/drums/kit.json').then((r) => r.json()),
  ]);
  function audio() {
    if (!ac) {
      ac = new (window.AudioContext || window.webkitAudioContext)();
      const comp = ac.createDynamicsCompressor();
      comp.threshold.value = -10;
      comp.ratio.value = 2.5;
      out = ac.createGain();
      out.gain.value = 0.85;
      out.connect(comp).connect(ac.destination);
    }
    if (ac.state === 'suspended') ac.resume();
    return ac;
  }
  function loadKit() {
    if (!kitLoading) {
      kitLoading = kitFiles.then(([data, map]) => audio().decodeAudioData(data.slice(0)).then((buffer) => {
        // the decoder may shift the file by a few milliseconds: start every stroke where its sound starts
        const ch = buffer.getChannelData(0), sr = buffer.sampleRate, slots = {};
        for (const [name, [start, dur]] of Object.entries(map)) {
          const a = Math.max(0, Math.floor((start - 0.06) * sr)), b = Math.min(ch.length, Math.floor((start + dur) * sr));
          let peak = 0;
          for (let i = a; i < b; i++) peak = Math.max(peak, Math.abs(ch[i]));
          let on = a;
          while (on < b && Math.abs(ch[on]) < peak * 0.08) on++;
          on = Math.max(a, on - Math.floor(0.002 * sr));
          slots[name] = { at: on / sr, dur: (b - on) / sr };
        }
        kit = { buffer, slots };
      }));
    }
    return kitLoading;
  }
  function choke(t) {
    if (!openHat) return;
    openHat.gain.setTargetAtTime(0.0001, t, 0.015);
    openHat = null;
  }
  // v: 1 normal, 2 accent, 3 ghost
  function hit(id, v, t) {
    if (!kit) return;
    let name = id + (v === 2 ? '_a' : v === 3 ? '_g' : '');
    let gain = 1;
    if (!kit.slots[name]) {
      name = id;
      gain = v === 2 ? 1.4 : v === 3 ? 0.3 : 1;
    }
    const slot = kit.slots[name];
    if (!slot) return;
    if (id === 'HH' || id === 'HF' || id === 'HO') choke(t);
    const src = ac.createBufferSource();
    src.buffer = kit.buffer;
    const g = ac.createGain();
    g.gain.value = gain;
    src.connect(g).connect(out);
    src.start(Math.max(t, ac.currentTime), slot.at, slot.dur);
    if (id === 'HO') openHat = g;
  }
  const flam = (id, t) => hit(id, 3, t - 0.028);

  // One thing plays at a time: the step grid, a library card or the recording.
  let current = null;
  function stopAll() { if (current) current.stop(); }

  // A look-ahead loop over the steps of a pattern; draw(step) is called when a step is heard.
  function makeLoop({ steps, stepDur, play, draw, end, loops = Infinity }) {
    let next = 0, n = 0, timer = 0, raf = 0, running = false;
    const queue = [];
    const self = {
      start() {
        stopAll();
        audio();
        if (!kit) { loadKit().then(() => self.start()); return; }
        current = self;
        running = true;
        n = 0;
        next = ac.currentTime + 0.06;
        queue.length = 0;
        const tick = () => {
          while (running && next < ac.currentTime + 0.12) {
            if (n >= loops * steps()) { break; }
            const s = n % steps();
            play(s, next);
            queue.push({ s, t: next });
            next += stepDur();
            n++;
          }
          if (n >= loops * steps() && queue.length === 0) self.stop(true);
        };
        const frame = () => {
          if (!running) return;
          while (queue.length && queue[0].t <= ac.currentTime) draw(queue.shift().s);
          raf = requestAnimationFrame(frame);
        };
        tick();
        timer = setInterval(tick, 25);
        raf = requestAnimationFrame(frame);
      },
      stop(finished) {
        if (!running) return;
        running = false;
        clearInterval(timer);
        cancelAnimationFrame(raf);
        if (current === self) current = null;
        end(finished === true);
      },
      get running() { return running; },
    };
    return self;
  }

  // ------------------------------------------------------------------ patterns
  // Lines are tab strings: '-' nothing, 'x' stroke, 'X' accent, 'g' ghost, 'f' flam. 16 steps a bar unless "steps".
  const H8 = 'x-x-x-x-x-x-x-x-', BB = '----x-------x---', K13 = 'x-------x-------';
  const P = {
    'rock-8': { en: 'Rock beat (8th-note hi-hat)', ko: '록 비트 (8분음표 하이햇)', HH: H8, SD: BB, BD: K13 },
    'rock-8-push': { en: 'Rock beat with pushed kick', ko: '록 비트 (킥 당김음)', HH: H8, SD: BB, BD: 'x-----x-x-------' },
    'rock-16': { en: 'Rock beat (16th-note hi-hat)', ko: '록 비트 (16분음표 하이햇)', HH: 'xxxxxxxxxxxxxxxx', SD: BB, BD: K13 },
    quarter: { en: 'Rock beat (quarter-note hi-hat)', ko: '록 비트 (4분음표 하이햇)', HH: 'x---x---x---x---', SD: BB, BD: K13 },
    'crash-rock-8': { en: 'Rock beat starting with a crash', ko: '크래시로 시작하는 록 비트', CC: 'x---------------', HH: '--x-x-x-x-x-x-x-', SD: BB, BD: K13 },
    'ride-8': { en: 'Rock beat on the ride', ko: '라이드 록 비트', RD: H8, SD: BB, BD: K13 },
    'half-time': { en: 'Half-time beat', ko: '하프타임 비트', HH: H8, SD: '--------x-------', BD: 'x---------------' },
    'four-floor': { en: 'Four on the floor', ko: '포 온 더 플로어', HH: H8, SD: BB, BD: 'x---x---x---x---' },
    disco: { en: 'Disco beat (open hi-hat off the beat)', ko: '디스코 비트 (엇박 오픈 하이햇)', HH: 'x---x---x---x---', HO: '--x---x---x---x-', SD: BB, BD: 'x---x---x---x---' },
    motown: { en: 'Motown beat (snare on every beat)', ko: '모타운 비트 (매 박 스네어)', HH: H8, SD: 'x---x---x---x---', BD: K13 },
    'bossa-nova': { en: 'Bossa nova', ko: '보사노바', HH: H8, XS: 'x-----x-----x---', BD: 'x-----x-x-----x-' },
    reggae: { en: 'Reggae one drop', ko: '레게 원 드롭', HH: H8, XS: '--------x-------', BD: '--------x-------' },
    shuffle: { en: 'Shuffle beat', ko: '셔플 비트', steps: 12, HH: 'x-xx-xx-xx-x', SD: '---x-----x--', BD: 'x-----x-----' },
    'triplet-feel': { en: 'Slow blues beat (triplet hi-hat)', ko: '슬로 블루스 비트 (셋잇단 하이햇)', steps: 12, HH: 'xxxxxxxxxxxx', SD: '---x-----x--', BD: 'x-----x-----' },
    'jazz-ride': { en: 'Jazz ride (swing)', ko: '재즈 라이드 (스윙)', steps: 12, RD: 'x--x-xx--x-x', HF: '---x-----x--' },
    'six-eight': { en: '6/8 beat', ko: '6/8 비트', steps: 12, beat: 6, HH: 'x-x-x-x-x-x-', SD: '------x-----', BD: 'x-----------' },
    'snare-fill': { en: 'Fill: 16th notes on the snare', ko: '필인: 스네어 16분음표', SD: 'xxxxxxxxxxxxxxxx' },
    'tom-fill': { en: 'Fill: 16th notes down the toms', ko: '필인: 탐 16분음표', SD: 'xxxx------------', T1: '----xxxx--------', T2: '--------xxxx----', FT: '------------xxxx' },
    'half-bar-fill': { en: 'Fill: beat, then a fill on 3 and 4', ko: '필인: 두 박 비트 뒤 두 박 필인', HH: 'x-x-x-x---------', SD: '----x---xxxx----', T1: '------------xx--', FT: '--------------xx', BD: 'x---------------' },
    'triplet-fill': { en: 'Fill: triplets down the toms', ko: '필인: 탐 셋잇단', steps: 12, SD: 'xxx---------', T1: '---xxx------', T2: '------xxx---', FT: '---------xxx' },
    'r-single': { en: 'Single stroke roll', ko: '싱글 스트로크 롤', stick: 'RL', SD: 'xxxxxxxxxxxxxxxx' },
    'r-double': { en: 'Double stroke roll', ko: '더블 스트로크 롤', stick: 'RRLL', SD: 'xxxxxxxxxxxxxxxx' },
    'r-paradiddle': { en: 'Single paradiddle', ko: '싱글 패러디들', stick: 'RLRRLRLL', SD: 'XxxxXxxxXxxxXxxx' },
    'r-triplets': { en: 'Single stroke triplets', ko: '싱글 스트로크 셋잇단', steps: 12, stick: 'RL', SD: 'xxxxxxxxxxxx' },
    'r-double-paradiddle': { en: 'Double paradiddle', ko: '더블 패러디들', steps: 12, stick: 'RLRLRRLRLRLL', SD: 'XxxxxxXxxxxx' },
    'r-paradiddle-diddle': { en: 'Paradiddle-diddle', ko: '패러디들 디들', steps: 12, stick: 'RLRRLL', SD: 'XxxxxxXxxxxx' },
    'r-flams': { en: 'Flams', ko: '플램', stick: 'RL', stickEvery: 4, SD: 'f---f---f---f---' },
    'r-flam-tap': { en: 'Flam tap', ko: '플램 탭', stick: 'RRLL', SD: 'fxfxfxfxfxfxfxfx' },
    'r-flam-accent': { en: 'Flam accent', ko: '플램 악센트', steps: 12, stick: 'RL', SD: 'fxxfxxfxxfxx' },
  };
  const CATS = {
    groove: ['rock-8', 'rock-8-push', 'rock-16', 'quarter', 'crash-rock-8', 'ride-8', 'half-time', 'four-floor', 'disco', 'motown', 'bossa-nova', 'reggae', 'shuffle', 'triplet-feel', 'jazz-ride', 'six-eight'],
    fill: ['snare-fill', 'tom-fill', 'half-bar-fill', 'triplet-fill'],
    rudiment: ['r-single', 'r-double', 'r-paradiddle', 'r-triplets', 'r-double-paradiddle', 'r-paradiddle-diddle', 'r-flams', 'r-flam-tap', 'r-flam-accent'],
  };
  const LINE_ORDER = ['CC', 'RD', 'HH', 'HO', 'T1', 'T2', 'XS', 'SD', 'FT', 'BD', 'HF'];
  const COLOR = { CC: 'var(--hat)', RD: 'var(--hat)', HH: 'var(--hat)', HO: 'var(--hat)', HF: 'var(--hat)', T1: 'var(--snare)', T2: 'var(--snare)', XS: 'var(--snare)', SD: 'var(--snare)', FT: 'var(--snare)', BD: 'var(--kick)' };
  const val = (ch) => (ch === '-' ? 0 : ch === 'X' ? 2 : ch === 'g' ? 3 : ch === 'f' ? 4 : 1);
  const stepsOf = (p) => p.steps || 16;
  // the length of a step in seconds at a tempo: sixteenths, or triplet eighths for 12-step bars (6/8: eighths)
  const stepLen = (p, bpm) => (p.beat === 6 ? 60 / bpm / 2 : stepsOf(p) === 12 ? 60 / bpm / 3 : 60 / bpm / 4);

  // ------------------------------------------------------------------ the step grid (two measures)
  const ROWS = [
    { id: 'CC', en: 'Crash cymbal', ko: '크래시 심벌' },
    { id: 'HH', en: 'Closed hi-hat', ko: '클로즈드 하이햇' },
    { id: 'HO', en: 'Open hi-hat', ko: '오픈 하이햇' },
    { id: 'RD', en: 'Ride cymbal', ko: '라이드 심벌' },
    { id: 'T1', en: 'High tom', ko: '하이 탐' },
    { id: 'T2', en: 'Low tom', ko: '로우 탐' },
    { id: 'SD', en: 'Snare', ko: '스네어' },
    { id: 'FT', en: 'Floor tom', ko: '플로어 탐' },
    { id: 'BD', en: 'Bass drum', ko: '베이스 드럼' },
  ];
  const PRESETS = [
    { en: 'Rock', ko: '록', m: ['crash-rock-8', 'rock-8-push'] },
    { en: 'Disco', ko: '디스코', m: ['disco', 'disco'] },
    { en: 'Half-time', ko: '하프타임', m: ['half-time', 'half-time'], ghost: { SD: [6, 14] } },
    { en: 'Funk', ko: '펑크', funk: true },
    { en: 'Fill', ko: '필인', m: ['rock-8', 'half-bar-fill'] },
  ];
  const song = [{}, {}];            // measure -> row -> [16 values]
  let meas = 0, preset = 0;
  const empty = () => Array(16).fill(0);
  function loadPattern(m, id) {
    const p = P[id];
    for (const r of ROWS) song[m][r.id] = p[r.id] ? [...p[r.id]].map(val) : empty();
  }
  function loadPreset(i) {
    preset = i;
    const pr = PRESETS[i];
    if (pr.funk) {
      for (const m of [0, 1]) {
        for (const r of ROWS) song[m][r.id] = empty();
        song[m].HH = [...'XxxxXxxxXxxxXxxx'].map(val);
        song[m].SD = [...'-g--X--g-g--X--g'].map(val);
        song[m].BD = [...'x-x----x--x-----'].map(val);
      }
      song[1].HH[14] = 0;
      song[1].HO = empty();
      song[1].HO[14] = 1;
    } else {
      pr.m.forEach((id, m) => loadPattern(m, id));
      if (pr.ghost) for (const [row, idx] of Object.entries(pr.ghost)) for (const m of [0, 1]) idx.forEach((s) => { song[m][row][s] = 3; });
    }
    drawGrid();
    drawPresets();
  }

  const gridEl = $('#grid-cells');
  const cells = {};
  const numEls = [];
  function buildGrid() {
    gridEl.append(document.createElement('span'));
    for (let s = 0; s < 16; s++) {
      const n = document.createElement('span');
      n.className = 'num' + (s % 4 === 0 && s ? ' q' : '');
      n.textContent = s % 4 === 0 ? s / 4 + 1 : '';
      gridEl.append(n);
      numEls.push(n);
    }
    for (const r of ROWS) {
      const lbl = document.createElement('span');
      lbl.className = 'lbl';
      lbl.dataset.row = r.id;
      gridEl.append(lbl);
      cells[r.id] = [];
      for (let s = 0; s < 16; s++) {
        const b = document.createElement('button');
        b.type = 'button';
        b.className = 'cell' + (s % 4 === 0 && s ? ' q' : '');
        b.style.setProperty('--c', COLOR[r.id]);
        b.addEventListener('click', () => {
          const row = song[meas][r.id];
          row[s] = row[s] ? 0 : 1;
          if (r.id === 'HH' && row[s]) song[meas].HO[s] = 0;
          if (r.id === 'HO' && row[s]) song[meas].HH[s] = 0;
          preset = -1;
          drawGrid();
          drawPresets();
          if (row[s]) { audio(); loadKit().then(() => hit(r.id, row[s], ac.currentTime)); }
        });
        b.addEventListener('contextmenu', (e) => {
          e.preventDefault();
          const row = song[meas][r.id];
          // accent -> ghost note -> normal, as in KitNote
          row[s] = row[s] === 0 || row[s] === 1 ? 2 : row[s] === 2 ? 3 : 1;
          preset = -1;
          drawGrid();
          drawPresets();
          audio();
          loadKit().then(() => hit(r.id, row[s], ac.currentTime));
        });
        gridEl.append(b);
        cells[r.id].push(b);
      }
    }
  }
  function labels() {
    for (const el of $$('.grid .lbl')) {
      const r = ROWS.find((x) => x.id === el.dataset.row);
      el.textContent = T(r.en, r.ko);
    }
    for (const r of ROWS) cells[r.id].forEach((b, s) => b.setAttribute('aria-label', `${T(r.en, r.ko)} ${s + 1}`));
  }
  let col = -1;
  function drawGrid() {
    for (const r of ROWS) {
      cells[r.id].forEach((b, s) => {
        const v = song[meas][r.id][s];
        b.classList.toggle('on', v === 1 || v === 2);
        b.classList.toggle('acc', v === 2);
        b.classList.toggle('ghost', v === 3);
        b.classList.toggle('now', s === col);
        b.setAttribute('aria-pressed', v ? 'true' : 'false');
      });
    }
    $('#meas').textContent = meas + 1;
    $('#m-prev').disabled = meas === 0;
    $('#m-next').disabled = meas === 1;
    $('#copy-next').disabled = meas === 1;
    $('#tab').innerHTML = tabText(true);
  }
  // KitNote's text tab: one line per drum, both measures
  function tabText(mark) {
    const lines = [];
    const order = [['CC', ['CC']], ['RD', ['RD']], ['HH', ['HH', 'HO']], ['T1', ['T1']], ['T2', ['T2']], ['SD', ['SD']], ['FT', ['FT']], ['BD', ['BD']]];
    for (const [name, rows] of order) {
      let used = false;
      const bars = [0, 1].map((m) => {
        const chars = [];
        for (let s = 0; s < 16; s++) {
          let ch = '-';
          for (const r of rows) {
            const v = song[m][r][s];
            if (!v) continue;
            used = true;
            const open = r === 'HO';
            const cym = name === 'CC' || name === 'RD' || name === 'HH';
            ch = v === 3 ? 'g' : open ? (v === 2 ? 'O' : 'o') : cym ? (v === 2 ? 'X' : 'x') : (v === 2 ? 'O' : 'o');
          }
          chars.push(mark && m === playMeas && s === col && playing() ? `<b>${ch}</b>` : ch);
        }
        return chars.join('');
      });
      if (used || ['HH', 'SD', 'BD'].includes(name)) lines.push(`${name}|${bars[0]}|${bars[1]}|`);
    }
    return lines.join('\n');
  }
  function drawPresets() {
    const el = $('#presets');
    el.innerHTML = '';
    PRESETS.forEach((p, i) => {
      const b = document.createElement('button');
      b.type = 'button';
      b.textContent = T(p.en, p.ko);
      b.setAttribute('aria-pressed', i === preset ? 'true' : 'false');
      b.addEventListener('click', () => loadPreset(i));
      el.append(b);
    });
  }

  let bpm = 112, playMeas = 0;
  const playing = () => gridLoop.running;
  const gridLoop = makeLoop({
    steps: () => 32,
    stepDur: () => 60 / bpm / 4,
    play(s, t) {
      const m = s >> 4, st = s & 15;
      for (const r of ROWS) {
        const v = song[m][r.id][st];
        if (v) hit(r.id, v, t);
      }
    },
    draw(s) {
      playMeas = s >> 4;
      if (meas !== playMeas) meas = playMeas;
      col = s & 15;
      drawGrid();
    },
    end() {
      col = -1;
      $('#play').classList.remove('on');
      $('#play').setAttribute('aria-label', T('Play', '재생'));
      drawGrid();
    },
  });
  $('#play').addEventListener('click', () => {
    if (gridLoop.running) { gridLoop.stop(); return; }
    gridLoop.start();
    $('#play').classList.add('on');
    $('#play').setAttribute('aria-label', T('Stop', '정지'));
  });
  $('#m-prev').addEventListener('click', () => { meas = 0; drawGrid(); });
  $('#m-next').addEventListener('click', () => { meas = 1; drawGrid(); });
  $('#copy-next').addEventListener('click', () => {
    for (const r of ROWS) song[1][r.id] = [...song[0][r.id]];
    meas = 1;
    preset = -1;
    drawGrid();
    drawPresets();
  });
  $('#clear').addEventListener('click', () => {
    for (const r of ROWS) song[meas][r.id] = empty();
    preset = -1;
    drawGrid();
    drawPresets();
  });
  const setBpm = (v) => { bpm = Math.max(60, Math.min(200, v)); $('#bpm').textContent = bpm; };
  $('#t-down').addEventListener('click', () => setBpm(bpm - 4));
  $('#t-up').addEventListener('click', () => setBpm(bpm + 4));
  $('#copy-tab').addEventListener('click', async () => {
    const label = $('#copy-tab span');
    try {
      await navigator.clipboard.writeText(tabText(false) + '\n');
      label.textContent = T('Copied', '복사됨');
    } catch (e) {
      label.textContent = T('Select the tab to copy', '탭을 선택해 복사하세요');
    }
    setTimeout(() => { label.innerHTML = lang === 'ko' ? label.dataset.ko : label.dataset.en; }, 1600);
  });
  // stop when the grid leaves the screen
  new IntersectionObserver((es) => { if (!es[0].isIntersecting && gridLoop.running) gridLoop.stop(); }).observe($('#machine'));

  // the grid's playhead walks in time, silently, until someone presses play
  if (!reduced) {
    let idle = 0, last = 0;
    const walk = (now) => {
      if (!gridLoop.running && now - last >= 60000 / bpm / 4) {
        last = now;
        col = idle;
        idle = (idle + 1) % 16;
        for (const r of ROWS) cells[r.id].forEach((b, s) => b.classList.toggle('now', s === col));
      }
      requestAnimationFrame(walk);
    };
    requestAnimationFrame(walk);
  }

  // ------------------------------------------------------------------ the pattern library
  let cat = 'groove';
  const cardsEl = $('#lib-cards');
  let cardLoop = null;
  function patternRows(p) { return LINE_ORDER.filter((id) => p[id]); }
  function canOpen(p) { return stepsOf(p) === 16 && !p.XS && !p.HF && !p.stick; }
  function renderCards() {
    if (cardLoop) cardLoop.stop();
    cardsEl.innerHTML = '';
    for (const id of CATS[cat]) {
      const p = P[id];
      const n = stepsOf(p);
      const per = p.beat === 6 ? 6 : n === 12 ? 3 : 4;
      const card = document.createElement('div');
      card.className = 'pcard';
      card.setAttribute('role', 'button');
      card.tabIndex = 0;
      const t = document.createElement('div');
      t.className = 't';
      t.textContent = T(p.en, p.ko);
      card.append(t);
      let go = null;
      if (canOpen(p)) {
        go = document.createElement('button');
        go.type = 'button';
        go.className = 'go';
        go.textContent = T('Open in step grid', '스텝 그리드로');
        go.addEventListener('click', (e) => {
          e.stopPropagation();
          loadPattern(0, id);
          loadPattern(1, id);
          preset = -1;
          meas = 0;
          drawGrid();
          drawPresets();
          document.getElementById('grid').scrollIntoView({ behavior: reduced ? 'auto' : 'smooth' });
        });
      }
      const mini = document.createElement('div');
      mini.className = 'mini';
      mini.style.setProperty('--n', n);
      const marks = [];
      if (p.stick) {
        mini.append(document.createElement('span'));
        const every = p.stickEvery || 1;
        let k = 0;
        for (let s = 0; s < n; s++) {
          const st = document.createElement('span');
          const ch = p.SD[s];
          st.className = 'st' + (ch === 'X' ? ' a' : '') + (s % per === 0 && s ? ' q' : '');
          if (ch !== '-' && s % every === 0) {
            const letter = p.stick[k % p.stick.length];
            st.textContent = ch === 'f' ? (letter === 'R' ? 'lR' : 'rL') : letter;
            k++;
          }
          mini.append(st);
        }
      }
      for (const row of patternRows(p)) {
        const ml = document.createElement('span');
        ml.className = 'ml';
        ml.textContent = row;
        mini.append(ml);
        for (let s = 0; s < n; s++) {
          const i = document.createElement('i');
          const v = val(p[row][s]);
          i.className = (v ? 'on' : '') + (v === 2 ? ' acc' : '') + (s % per === 0 && s ? ' q' : '');
          i.style.setProperty('--c', COLOR[row]);
          i.dataset.s = s;
          mini.append(i);
          marks.push(i);
        }
      }
      card.append(mini);
      if (canOpen(p)) card.append(go);
      const loop = makeLoop({
        steps: () => n,
        stepDur: () => stepLen(p, p.stick ? 96 : 104),
        loops: 4,
        play(s, tt) {
          for (const row of patternRows(p)) {
            const v = val(p[row][s]);
            if (!v) continue;
            if (v === 4) { flam(row, tt); hit(row, 2, tt); } else hit(row, v, tt);
          }
        },
        draw(s) { marks.forEach((m) => m.classList.toggle('now', +m.dataset.s === s)); },
        end() { marks.forEach((m) => m.classList.remove('now')); card.classList.remove('on'); },
      });
      const toggle = () => {
        if (loop.running) { loop.stop(); return; }
        loop.start();
        cardLoop = loop;
        card.classList.add('on');
      };
      card.addEventListener('click', toggle);
      card.addEventListener('keydown', (e) => { if (e.key === 'Enter' || e.key === ' ') { e.preventDefault(); toggle(); } });
      cardsEl.append(card);
    }
  }
  // The library's tabs: the three pattern lists, then the other features as panels of the same popup.
  const PANELS = ['audio', 'score', 'ai'];
  function selectTab(name) {
    if (name === cat) return;
    cat = name;
    stopAll();
    $$('#lib-tabs button').forEach((x) => x.setAttribute('aria-selected', x.dataset.cat === name ? 'true' : 'false'));
    $('#lib-cards-wrap').hidden = PANELS.includes(name);
    PANELS.forEach((id) => { document.getElementById(id).hidden = id !== name; });
    if (CATS[name]) renderCards();
    spyMark();
  }
  for (const b of $$('#lib-tabs button')) b.addEventListener('click', () => selectTab(b.dataset.cat));
  // the top bar's links to a feature open its tab
  for (const a of $$('a[href="#patterns"], a[href="#audio"], a[href="#score"], a[href="#ai"]')) {
    a.addEventListener('click', (e) => {
      e.preventDefault();
      const name = a.hash === '#patterns' ? (CATS[cat] ? cat : 'groove') : a.hash.slice(1);
      selectTab(name);
      $('#library').scrollIntoView({ behavior: reduced ? 'auto' : 'smooth', block: 'start' });
      history.replaceState(null, '', a.hash);
    });
  }
  PANELS.forEach((id) => { document.getElementById(id).hidden = true; });

  // ------------------------------------------------------------------ a recording becomes strokes
  // demo-groove.mp3 is four bars KitNote played at 120 BPM: three bars of the rock beat, then the half-bar fill.
  const FOUND_ROWS = [['CY', 'var(--hat)'], ['SD', 'var(--snare)'], ['TT', 'var(--snare)'], ['BD', 'var(--kick)']];
  const groove = { CY: [], SD: [], TT: [], BD: [] };
  for (let b = 0; b < 4; b++) {
    const p = b < 3 ? P['rock-8'] : P['half-bar-fill'];
    for (let s = 0; s < 16; s++) {
      if (p.HH && p.HH[s] !== '-') groove.CY.push(b * 16 + s);
      if (p.SD && p.SD[s] !== '-') groove.SD.push(b * 16 + s);
      if ((p.T1 && p.T1[s] !== '-') || (p.FT && p.FT[s] !== '-')) groove.TT.push(b * 16 + s);
      if (p.BD && p.BD[s] !== '-') groove.BD.push(b * 16 + s);
    }
  }
  const foundEl = $('#found');
  const foundCells = {};
  for (const [row, c] of FOUND_ROWS) {
    const ml = document.createElement('span');
    ml.className = 'ml';
    ml.textContent = row;
    foundEl.append(ml);
    foundCells[row] = [];
    for (let s = 0; s < 64; s++) {
      const i = document.createElement('i');
      i.className = s % 16 === 0 && s ? 'bar' : '';
      i.style.setProperty('--c', c);
      foundEl.append(i);
      foundCells[row].push(i);
    }
  }
  const wave = $('#wave'), wctx = wave.getContext('2d');
  let buffer = null, peaks = null, recSrc = null, recStart = 0, recRaf = 0;
  const STEP = 0.125;  // a sixteenth at 120 BPM
  async function loadRec() {
    if (buffer) return buffer;
    const data = await fetch('assets/demo-groove.mp3').then((r) => r.arrayBuffer());
    buffer = await audio().decodeAudioData(data);
    const ch = buffer.getChannelData(0), cols = 280, per = Math.floor(ch.length / cols);
    peaks = [];
    for (let c = 0; c < cols; c++) {
      let m = 0;
      for (let i = c * per; i < (c + 1) * per; i += 8) m = Math.max(m, Math.abs(ch[i]));
      peaks.push(m);
    }
    return buffer;
  }
  function drawWave(pos) {
    const w = wave.width, h = wave.height;
    wctx.clearRect(0, 0, w, h);
    if (!peaks) {
      wctx.fillStyle = '#E3E1D9';
      wctx.fillRect(0, h / 2 - 1, w, 2);
      return;
    }
    const bw = w / peaks.length, max = Math.max(...peaks);
    peaks.forEach((p, i) => {
      const x = i * bw, v = Math.max(2, (p / max) * (h - 8));
      wctx.fillStyle = x / w <= pos ? '#3F6F5E' : '#C9C6BB';
      wctx.fillRect(x + 0.5, (h - v) / 2, Math.max(1, bw - 1.5), v);
    });
  }
  function setFound(upto) {
    for (const [row] of FOUND_ROWS) {
      foundCells[row].forEach((c, s) => c.classList.toggle('on', s <= upto && groove[row].includes(s)));
    }
  }
  const recBtn = $('#rec-play'), recStatus = $('#rec-status');
  const recCtl = {
    running: false,
    stop(finished) {
      if (!this.running) return;
      this.running = false;
      try { recSrc.stop(); } catch (e) { /* already ended */ }
      cancelAnimationFrame(recRaf);
      if (current === recCtl) current = null;
      recBtn.classList.remove('on');
      if (finished) {
        setFound(63);
        drawWave(1);
        recStatus.innerHTML = T('<b>4 measures</b> · 4/4 · 120 BPM — three bars of the rock beat, then a fill. (Demo)', '<b>4마디</b> · 4/4 · 120 BPM — 록 비트 세 마디, 그리고 필인. (데모)');
      }
    },
  };
  recBtn.addEventListener('click', async () => {
    if (recCtl.running) { recCtl.stop(); return; }
    stopAll();
    recBtn.classList.add('on');
    recStatus.textContent = T('Listening…', '듣는 중…');
    await loadRec();
    current = recCtl;
    recCtl.running = true;
    setFound(-1);
    recSrc = ac.createBufferSource();
    recSrc.buffer = buffer;
    recSrc.connect(ac.destination);
    recStart = ac.currentTime + 0.05;
    recSrc.start(recStart);
    recSrc.onended = () => recCtl.stop(true);
    const frame = () => {
      if (!recCtl.running) return;
      const t = ac.currentTime - recStart;
      drawWave(t / buffer.duration);
      setFound(Math.floor(t / STEP) - 1);
      recRaf = requestAnimationFrame(frame);
    };
    recRaf = requestAnimationFrame(frame);
  });
  new IntersectionObserver((es) => {
    if (es[0].isIntersecting && !peaks) {
      fetch('assets/demo-groove.mp3').then((r) => r.arrayBuffer())
        .then((d) => new (window.OfflineAudioContext || window.webkitOfflineAudioContext)(1, 44100, 44100).decodeAudioData(d))
        .then((b) => {
          if (buffer) return;
          const ch = b.getChannelData(0), cols = 280, per = Math.floor(ch.length / cols);
          peaks = [];
          for (let c = 0; c < cols; c++) {
            let m = 0;
            for (let i = c * per; i < (c + 1) * per; i += 8) m = Math.max(m, Math.abs(ch[i]));
            peaks.push(m);
          }
          drawWave(0);
        })
        .catch(() => drawWave(0));
    }
  }, { rootMargin: '200px' }).observe(wave);
  drawWave(0);

  // ------------------------------------------------------------------ the AI terminal
  const CHATS = [
    {
      en: 'Make a drum chart in KitNote: pop rock at 120 BPM, intro 4, verse 8, chorus 8 on the ride, a fill at the end of each section. PDF and MIDI too.',
      ko: 'KitNote로 드럼 악보 만들어줘: 팝 록 120 BPM, 인트로 4마디, 벌스 8마디, 코러스 8마디는 라이드로, 구간 끝마다 필인. PDF랑 MIDI도.',
      calls: [['create_score', '20 measures · 4/4 · 120 BPM'], ['fill_pattern', 'rock-8 · ride-8 · tom-fill'], ['add_section', 'Intro · Verse · Chorus'], ['export_score', 'chart.pdf · chart.mid']],
    },
    {
      en: 'In song.kitnote, change measures 9–16 from hi-hat to ride and put a tom fill in the last measure.',
      ko: 'song.kitnote에서 9–16마디 하이햇을 라이드로 바꾸고 마지막 마디에 탐 필인 넣어줘.',
      calls: [['read_score', 'song.kitnote · 32 measures'], ['edit_score', 'm. 9–16 · hi-hat → ride'], ['fill_pattern', 'm. 16 · tom-fill'], ['view_score', 'page 1']],
    },
    {
      en: 'Make a drum score of song.mp3.',
      ko: 'song.mp3를 드럼 악보로 만들어줘.',
      calls: [['audio_engine', 'ready'], ['transcribe_audio', 'song.mp3 → song.kitnote'], ['read_score', '4/4 · 112 BPM · 68 measures'], ['open_in_kitnote', 'song.kitnote']],
    },
  ];
  const typed = $('#typed'), callsEl = $('#calls');
  let chat = 0, termOn = false, run = 0;
  const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
  async function term(id) {
    while (termOn && id === run) {
      const c = CHATS[chat % CHATS.length];
      const text = c[lang];
      callsEl.innerHTML = '';
      if (reduced) {
        typed.textContent = text;
      } else {
        for (let i = 1; i <= text.length; i++) {
          if (id !== run) return;
          typed.textContent = text.slice(0, i);
          await sleep(18 + Math.random() * 26);
        }
      }
      await sleep(450);
      for (const [k, [tool, note]] of c.calls.entries()) {
        if (id !== run) return;
        const li = document.createElement('li');
        li.innerHTML = `<b>kitnote · ${tool}</b><span>${note}</span>`;
        callsEl.append(li);
        requestAnimationFrame(() => li.classList.add('in'));
        await sleep(reduced ? 0 : 480);
        if (k === c.calls.length - 1) li.classList.add('done');
      }
      await sleep(2800);
      chat++;
    }
  }
  new IntersectionObserver((es) => {
    termOn = es[0].isIntersecting;
    if (termOn) term(++run);
  }, { threshold: 0.3 }).observe($('.term'));
  onLang.push(() => { if (termOn) term(++run); });

  // ------------------------------------------------------------------ downloads
  const plat = (navigator.userAgentData && navigator.userAgentData.platform) || navigator.platform || navigator.userAgent;
  const os = /mac|iphone|ipad/i.test(plat) ? 'mac' : /win/i.test(plat) ? 'win' : '';
  const files = {};
  function dlLabels() {
    const main = $('#dl-main-label'), alt = $('#dl-alt');
    if (os === 'win') {
      main.textContent = T('Download for Windows', 'Windows용 다운로드');
      alt.textContent = 'macOS';
      if (files.dmg) alt.href = files.dmg.browser_download_url;
    } else if (os === 'mac') {
      main.textContent = T('Download for macOS', 'macOS용 다운로드');
      alt.textContent = 'Windows';
      if (files.exe) alt.href = files.exe.browser_download_url;
    }
  }
  if (os) {
    const row = $(`.dl-row[data-os="${os}"]`);
    if (row) row.classList.add('mine');
  }
  onLang.push(dlLabels);
  fetch(`https://api.github.com/repos/${REPO}/releases/latest`, { headers: { Accept: 'application/vnd.github+json' } })
    .then((r) => (r.ok ? r.json() : Promise.reject(r.status)))
    .then((rel) => {
      const v = (rel.tag_name || '').replace(/^v/, '');
      if (v) { $('#ver').textContent = v; $('#ver2').textContent = v; }
      for (const a of rel.assets || []) {
        if (a.name.endsWith('-setup.exe')) files.exe = a;
        else if (a.name.endsWith('-win64.zip')) files.zip = a;
        else if (a.name.endsWith('.dmg')) files.dmg = a;
        else if (a.name.startsWith('SHA256SUMS')) files.sums = a;
      }
      const mb = (n) => `${Math.round(n / 1048576)} MB`;
      for (const k of ['exe', 'dmg', 'zip']) {
        if (!files[k]) continue;
        $(`#a-${k}`).href = files[k].browser_download_url;
        $(`#f-${k}`).textContent = `${files[k].name} · ${mb(files[k].size)}`;
      }
      if (files.sums) $('#sums').href = files.sums.browser_download_url;
      const mine = os === 'win' ? files.exe : os === 'mac' ? files.dmg : null;
      if (mine) $('#dl-main').href = mine.browser_download_url;
      dlLabels();
    })
    .catch(() => { /* the links stay on the releases page */ });

  // ------------------------------------------------------------------ start
  buildGrid();
  for (const r of ROWS) song[0][r.id] = empty(), song[1][r.id] = empty();
  onLang.push(labels, drawPresets, () => { if (CATS[cat]) renderCards(); }, () => drawGrid());
  applyLang();
  loadPreset(0);
  const start = location.hash.slice(1);
  if (PANELS.includes(start)) selectTab(start);
})();
