'use strict';

module.exports = function (nodecg) {
	const countdownReplicant = nodecg.Replicant('countdown', {defaultValue: {seconds: 60*60*10, active: false}});

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
}

