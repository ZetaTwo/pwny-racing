'use strict';
const express = require('express');
const app = express();

module.exports = function (nodecg) {
	const countdownReplicant = nodecg.Replicant('countdown', {defaultValue: {seconds: 60*60*10, active: false}});
	const gameStateReplicant = nodecg.Replicant('gamestate', {defaultValue: {started: false, finished: false, fireworks: false, winner: false}});
	const statusReplicant = nodecg.Replicant('statusSpan', { defaultValue: 'Hacking with Zeta Two' });
	const playerRoundsReplicant = nodecg.Replicant('playerRounds', { defaultValue: [
		[false, false, false, false, false],
		[false, false, false, false, false],
		[false, false, false, false, false],
		[false, false, false, false, false],
	]});

	function updateCountdown() {
		if(countdownReplicant.value.active) {
			countdownReplicant.value.seconds -= 1;
		}
		if(countdownReplicant.value.seconds <= 0) {
			countdownReplicant.value.seconds = 0;
			countdownReplicant.value.active = false;
		}
		setTimeout(updateCountdown, 1000);
	}
	updateCountdown();

	app.post('/pwnyracing/flag', (req, res) => {
		var is_winner = false;
		if(gameStateReplicant.value.started == true && gameStateReplicant.value.finished == false) {
			gameStateReplicant.value.finished = true;
			gameStateReplicant.value.fireworks = true;
			gameStateReplicant.value.winner = req.body.username;
			is_winner = true;
			console.log("Winner: %s", req.body.username);
		} else {
			console.log("Loser: %s", req.body.username);
		}

        res.json({"winner": is_winner});
	});

	app.post('/pwnyracing/roundflag', (req, res) => {
		req.body.username
	});

	app.post('/pwnyracing/stream/status', (req, res) => {
		statusReplicant.value = req.body.status;

        res.json({"status": "ok"});
    });

    nodecg.mount(app); // The route '/my-bundle/customroute` is now available
}

