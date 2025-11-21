import React, { useState, useEffect } from 'react';
import { GAME_STATE } from './game_state_enum.js';

function App() {
  // board size and game state
  const [size, setSize] = useState(4);
  const [gameState, setGameState] = useState(GAME_STATE.NOT_STARTED);

  // data from backend
  const [grid, setGrid] = useState([]);       // 2D array of letters
  const [game, setGame] = useState({});       // full JSON game object
  const [foundSolutions, setFoundSolutions] = useState([]); // player guesses (optional)
  const [allSolutions, setAllSolutions] = useState([]);     // backend solutions

  // helper: convert backend "foundwords" string → array of words
  const Convert = (s) => {
    if (!s) return [];
    // remove single quotes and brackets
    s = s.replace(/'/g, '');
    s = s.replace('[', '');
    s = s.replace(']', '');
    // split on commas, trim each word, drop empties
    const tokens = s
      .split(',')
      .map((token) => token.trim())
      .filter((token) => token !== '');
    return tokens;
  };

  // fetch a new game from Django whenever the game starts or size changes
  useEffect(() => {
    if (gameState === GAME_STATE.IN_PROGRESS) {
      // size is part of the path, e.g. /api/game_create/4/
      const url = `https://fractallocal-campusthink-8000.codio.io/api/game_create/${size}/`;

      fetch(url)
        .then((response) => response.json())
        .then((data) => {
          // save the whole game object
          setGame(data);

          // grid is stored as a string with single quotes
          // replace ' with " and parse into a JS array
          const s = data.grid.replace(/'/g, '"');
          setGrid(JSON.parse(s));

          // clear any previous solutions
          setFoundSolutions([]);
        })
        .catch((err) => {
          console.log('Error fetching game:', err.message);
        });
    }
  }, [gameState, size]);

  // whenever grid or game.foundwords change, recompute allSolutions
  useEffect(() => {
    if (typeof game.foundwords !== 'undefined') {
      const tmpAllSolutions = Convert(game.foundwords);
      setAllSolutions(tmpAllSolutions);
    }
  }, [grid, game.foundwords]);

  // button handler to start a new game
  const startGame = () => {
    setGameState(GAME_STATE.IN_PROGRESS);
  };

  // simple input so setSize and foundSolutions are used (and visible)
  const [currentWord, setCurrentWord] = useState('');

  const addFoundWord = () => {
    if (currentWord.trim() === '') return;
    setFoundSolutions((prev) => [...prev, currentWord.trim().toUpperCase()]);
    setCurrentWord('');
  };

  return (
    <div className="App" style={{ padding: '1rem' }}>
      <h1>Boggle (React + Django)</h1>

      <div style={{ marginBottom: '1rem' }}>
        <label>
          Board size:&nbsp;
          <input
            type="number"
            min="2"
            max="10"
            value={size}
            onChange={(e) => setSize(parseInt(e.target.value || '4', 10))}
          />
        </label>
        &nbsp;
        <button onClick={startGame}>Start Game</button>
      </div>

      <h2>Grid</h2>
      <pre>{JSON.stringify(grid, null, 2)}</pre>

      <h2>Your Found Words (Front-end)</h2>
      <div style={{ marginBottom: '0.5rem' }}>
        <input
          type="text"
          value={currentWord}
          onChange={(e) => setCurrentWord(e.target.value)}
          placeholder="type a word"
        />
        <button onClick={addFoundWord}>Add Word</button>
      </div>
      <ul>
        {foundSolutions.map((word, i) => (
          <li key={`found-${i}`}>{word}</li>
        ))}
      </ul>

      <h2>All Solutions from Backend</h2>
      <ul>
        {allSolutions.map((word, i) => (
          <li key={`backend-${i}`}>{word}</li>
        ))}
      </ul>
    </div>
  );
}

export default App;
