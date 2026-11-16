////////////////// les bases ///////////////////////////
let game = null;
const EMPTY = 0, TREE = 1, TENT = 2, GRASS = 3, BIGTREE = 4;


//////////////////// les prérequis /////////////////////
const images = {};
let imagesLoaded = 0;
const totalImages = 5; 

// fonction qui se lance par default
function loadImage(name, src) {
    images[name] = new Image();
    images[name].onload = () => {
        imagesLoaded++;
        //si tout est load alors on lance le jeu
        if (imagesLoaded === totalImages) {
            console.log("image charger");
            startGame();
        }
    };
    images[name].src = src;
}






/////////////parametre////////////////////////////////

////images
//chargement des images
loadImage('tree', '../res/tree.png');
loadImage('tent', '../res/tent.png');
loadImage('grass', '../res/grass.png');
loadImage('bigtree', '../res/bigtree.png');
loadImage('background', '../res/background.png');
///draw
//taille de l'image par rapport a la case
const imgSize=0.8

//lie un nom a une image (ne pas toucher)
const names = { [TREE]: 'tree', [TENT]: 'tent', [GRASS]: 'grass', [BIGTREE]: 'bigtree' };


////grille
//couleur
const grille_color='#000';
//epaisseur
const grille_ep=1;

////text nb tents
//couleur du text (nb tents attendu)
const text_color='#ffffff';
//taille et tipo
const text_type='20px Arial';










/////////////////// Lancement /////////////////////////

Module.onRuntimeInitialized = () => {
};

function startGame() {
    try {
        console.log("game start");
        game = Module.ccall('game_default', 'nb', [], []);
        resizeCanvas();
        attachButtonListeners();
    } catch (err) {
        console.error("Erreur start", err);
    }
}

function draw() {
    //on recup le canva
    const canvas = document.getElementById('theGame');
    const ctx = canvas.getContext('2d');
    const width = canvas.width;
    const height = canvas.height;

    // recupère le nombre de ligne et de colone 
    const nbRows = Module.ccall('game_nb_rows', 'nb', ['nb'], [game]);
    const nbCols = Module.ccall('game_nb_cols', 'nb', ['nb'], [game]);

    // +1 pour laisser de la place pour le nombre de tente attendu 
    const totalCols = nbCols + 1;
    const totalRows = nbRows + 1;

    //calcule de la taille d'une cellule
    const cellSize = Math.min(width / totalCols, height / totalRows);

    //backgrond
    ctx.drawImage(images.background, 0, 0, width, height);


    ////grille
    ctx.strokeStyle = grille_color;
    ctx.lineWidth = grille_ep;

    //lignes
    for (let i = 1; i <= totalRows; i++) {
        ctx.beginPath();
        ctx.moveTo(cellSize, i * cellSize);
        ctx.lineTo(totalCols * cellSize, i * cellSize);
        ctx.stroke();
    }
    //colones
    for (let j = 1; j <= totalCols; j++) {
        ctx.beginPath();
        ctx.moveTo(j * cellSize, cellSize);
        ctx.lineTo(j * cellSize, totalRows * cellSize);
        ctx.stroke();
    }
    

    // images
    for (let i = 0; i < nbRows; i++) {
        for (let j = 0; j < nbCols; j++) {
            const square = Module.ccall('game_get_square', 'nb', ['nb', 'nb', 'nb'], [game, i, j]);
            const x = (j + 1) * cellSize;
            const y = (i + 1) * cellSize;
            drawSymbol(ctx, x, y, cellSize, square);
        }
    }

    // nb tents
    ctx.fillStyle = text_color;
    ctx.font = text_type;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';

    //lignes
    for (let i = 0; i < nbRows; i++) {
        const expected = Module.ccall('game_get_expected_nb_tents_row', 'nb', ['nb', 'nb'], [game, i]);
        ctx.fillText(expected.toString(), cellSize / 2, (i + 1) * cellSize + cellSize / 2);
    }

    //colones
    for (let j = 0; j < nbCols; j++) {
        const expected = Module.ccall('game_get_expected_nb_tents_col', 'nb', ['nb', 'nb'], [game, j]);
        ctx.fillText(expected.toString(), (j + 1) * cellSize + cellSize / 2, cellSize / 2);
    }

    // affiche le message de victoire
    if (isGameWon()) {
        const boxWidth = width * 0.7;
        const boxHeight = height * 0.18;
        const boxX = (width - boxWidth) / 2;
        const boxY = (height - boxHeight) / 2;

        ctx.fillStyle = 'rgba(0, 0, 0, 0.8)';
        ctx.fillRect(boxX, boxY, boxWidth, boxHeight);
        ctx.strokeStyle = '#FFD700';
        ctx.lineWidth = 4;
        ctx.strokeRect(boxX, boxY, boxWidth, boxHeight);

        ctx.fillStyle = '#FFD700';
        ctx.font = 'bold 36px Arial';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText('Victoire !', width / 2, height / 2);
    }
}

function resizeCanvas(){
    const canvas = document.getElementById('theGame');
    const size = Math.min(window.innerWidth - 80, window.innerHeight - 400, 500);    //cavas carré avec de la place sur les côtés et pour les boutons textes etc, 500 = taille max
    canvas.width = size; canvas.height = size;
    if(game !== null) draw();
}

window.addEventListener('resize',resizeCanvas);         //s'active/reagit a chaque redimensionnement de la fenetre

function drawSymbol(ctx, x, y, size, square) {
    //si la case est vide on n'affiche rien
    if (square === EMPTY) return;


    const imageSize = size * imgSize;
    const offset = (size - imageSize) / 2;
    const name = names[square];

    ctx.drawImage(images[name], x + offset, y + offset, imageSize, imageSize);
}

function getSquare(i, j) {
    return Module.ccall('game_get_square', 'number', ['number', 'number', 'number'], [game, i, j]);
}

function countNeighbors(i, j, target, diag) {
    const directions = [
        [0, 1],
        [1, 0],
        [0, -1],
        [-1, 0],
    ];
    let count = 0;
    for (const [di, dj] of directions) {
        const ni = i + di;
        const nj = j + dj;
        if (ni >= 0 && ni < Module.ccall('game_nb_rows', 'number', ['number'], [game]) && nj >= 0 && nj < Module.ccall('game_nb_cols', 'number', ['number'], [game])) {
            if (getSquare(ni, nj) === target) count++;
        }
    }
    if (diag) {
        const diagDirs = [
            [1, 1],
            [1, -1],
            [-1, 1],
            [-1, -1],
        ];
        for (const [di, dj] of diagDirs) {
            const ni = i + di;
            const nj = j + dj;
            if (ni >= 0 && ni < Module.ccall('game_nb_rows', 'number', ['number'], [game]) && nj >= 0 && nj < Module.ccall('game_nb_cols', 'number', ['number'], [game])) {
                if (getSquare(ni, nj) === target) count++;
            }
        }
    }
    return count;
}

function hasNeighbor(i, j, target, diag) {
    return countNeighbors(i, j, target, diag) > 0;
}

function getCurrentTentsRow(i) {
    const nbCols = Module.ccall('game_nb_cols', 'number', ['number'], [game]);
    let count = 0;
    for (let j = 0; j < nbCols; j++) {
        if (getSquare(i, j) === TENT) count++;
    }
    return count;
}

function getCurrentTentsCol(j) {
    const nbRows = Module.ccall('game_nb_rows', 'number', ['number'], [game]);
    let count = 0;
    for (let i = 0; i < nbRows; i++) {
        if (getSquare(i, j) === TENT) count++;
    }
    return count;
}

function isGameWon() {
    //vérifie si la partie est gagné ou non
    const nbRows = Module.ccall('game_nb_rows', 'number', ['number'], [game]);
    const nbCols = Module.ccall('game_nb_cols', 'number', ['number'], [game]);
    let totalTent = 0;
    let totalTree = 0;

    for (let i = 0; i < nbRows; i++) {
        if (getCurrentTentsRow(i) !== Module.ccall('game_get_expected_nb_tents_row', 'number', ['number', 'number'], [game, i])) {
            return false;
        }
    }
    for (let j = 0; j < nbCols; j++) {
        if (getCurrentTentsCol(j) !== Module.ccall('game_get_expected_nb_tents_col', 'number', ['number', 'number'], [game, j])) {
            return false;
        }
    }

    for (let i = 0; i < nbRows; i++) {
        for (let j = 0; j < nbCols; j++) {
            const square = getSquare(i, j);
            if (square === TENT) {
                totalTent++;
                if (hasNeighbor(i, j, TENT, true)) return false;
                if (!hasNeighbor(i, j, TREE, false) && !hasNeighbor(i, j, BIGTREE, false)) return false;
            }
            if (square === TREE) {
                totalTree++;
                if (countNeighbors(i, j, TENT, false) === 0) return false;
            }
            if (square === BIGTREE) {
                totalTree += 2;
                if (countNeighbors(i, j, TENT, false) !== 2) return false;
            }
        }
    }

    return totalTent === totalTree;
}


///////////////////////Event////////////////////////////////////////////

function attachButtonListeners() {
    const canvas = document.getElementById('theGame');
    const btnRestart = document.getElementById('btnRestart');
    const btnUndo = document.getElementById('btnUndo');
    const btnRedo = document.getElementById('btnRedo');
    const btnSolve = document.getElementById('btnSolve');
    const btnRandom = document.getElementById('btnRandom');


    //click gauche
    canvas.addEventListener('click', (e) => {
        //recupère les coordoner de la souris
        const rect = canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        const nbRows = Module.ccall('game_nb_rows', 'nb', ['nb'], [game]);
        const nbCols = Module.ccall('game_nb_cols', 'nb', ['nb'], [game]);
        

        // Même calcul que dans draw()
        const totalCols = nbCols + 1;
        const totalRows = nbRows + 1;
        const cellSize = Math.min(canvas.width / totalCols, canvas.height / totalRows);

        // Calculer la cellule cliquée (en tenant compte de la colonne/ligne des nombres)
        const j = Math.floor(x / cellSize) - 1; // -1 car première colonne = nombres
        const i = Math.floor(y / cellSize) - 1; // -1 car première ligne = nombres

        if (i >= 0 && i < nbRows && j >= 0 && j < nbCols) {
            const square = Module.ccall('game_get_square', 'nb', ['nb', 'nb', 'nb'], [game, i, j]);
            if (square === EMPTY) {
                Module.ccall('game_play_move', 'void', ['nb', 'nb', 'nb', 'nb'], [game, i, j, TENT]);
            } else {
                Module.ccall('game_play_move', 'void', ['nb', 'nb', 'nb', 'nb'], [game, i, j, EMPTY]);
            }
            draw();
        }
    });



    //click droit
    canvas.addEventListener('contextmenu', (e) => {
        e.preventDefault(); //pour ne pas ouvrire le menu context
        //recupère les coordoner de la souris
        const rect = canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;

        const nbRows = Module.ccall('game_nb_rows', 'nb', ['nb'], [game]);
        const nbCols = Module.ccall('game_nb_cols', 'nb', ['nb'], [game]);
        

        // Même calcul que dans draw()
        const totalCols = nbCols + 1;
        const totalRows = nbRows + 1;
        const cellSize = Math.min(canvas.width / totalCols, canvas.height / totalRows);

        // Calculer la cellule cliquée (en tenant compte de la colonne/ligne des nombres)
        const j = Math.floor(x / cellSize) - 1; // -1 car première colonne = nombres
        const i = Math.floor(y / cellSize) - 1; // -1 car première ligne = nombres

        if (i >= 0 && i < nbRows && j >= 0 && j < nbCols) {
            const square = Module.ccall('game_get_square', 'nb', ['nb', 'nb', 'nb'], [game, i, j]);
            if (square === EMPTY) {
                Module.ccall('game_play_move', 'void', ['nb', 'nb', 'nb', 'nb'], [game, i, j, GRASS]);
            } else {
                Module.ccall('game_play_move', 'void', ['nb', 'nb', 'nb', 'nb'], [game, i, j, EMPTY]);
            }
            draw();
        }
    });




    /////////////////////boutons///////////////////////////////////////////

    //restart
    btnRestart.addEventListener('click', () => {
        Module.ccall('game_restart', 'void', ['nb'], [game]);
        draw();
    });

    //undo
    btnUndo.addEventListener('click', () => {
        Module.ccall('game_undo', 'void', ['nb'], [game]);
        draw();
    });

    //redo
    btnRedo.addEventListener('click', () => {
        Module.ccall('game_redo', 'void', ['nb'], [game]);
        draw();
    });

    //solve
    btnSolve.addEventListener('click', () => {
        Module.ccall('game_restart', 'void', ['nb'], [game]);
        Module.ccall('game_solve', 'bool', ['nb'], [game]);
        draw();
    });

    //random
    btnRandom.addEventListener('click', () => {
        Module.ccall('game_delete', 'void', ['nb'], [game]);
        game = Module.ccall('game_random', 'nb', ['nb', 'nb', 'nb', 'nb', 'nb', 'nb'], [8, 8, 0, 4, 2, 0]);
        draw();
    });

    ////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////