#!/usr/bin/env python3

from jinja2 import Environment, FileSystemLoader, StrictUndefined
import random
import string

ALPHABET = string.ascii_letters + string.digits

def generate_key():
    return ''.join([random.choice(ALPHABET) for _ in range(10)])

def generate_participant(participant):
    return {
        'participant_index': participant[0],
        'participant_name': participant[1],
        'participant_email': participant[2],
        'participant_key': participant[3] if len(participant) > 3 else generate_key(),
    }

env = Environment(
    loader=FileSystemLoader('.'),
    undefined = StrictUndefined
)

streamer_instructions_template = env.get_template('streamer-setup.md.tpl')
obs_settings_template = env.from_string('{{ participant_name }}: rtmp://stream.pwny.racing/participant_{{ participant_index }}/{{ participant_key }}?psk={{ episode_password }}')

episode = {
    'episode_number': 6,
    'episode_password': 'tLRnYNjw1vd1B8GK'
}

participants = [
    (0, 'acez', '?', 'f6Kxa0n8ChENaxtQ'),
    (1, 'blasty', '?', '47wCQdElhIPcpSJF'),
    (2, 'TBD', '?', 'nDyAwrKQ1r6eez99'),
    (3, 'TBD', '?', 'K9i0sKjCJ82kF1Q6'),
]

for participant in participants:
    print(streamer_instructions_template.render(dict(episode, **generate_participant(participant))))

for participant in participants:
    print(obs_settings_template.render(dict(episode, **generate_participant(participant))))
