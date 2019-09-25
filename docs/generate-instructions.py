#!/usr/bin/env python3

from jinja2 import Environment, FileSystemLoader, StrictUndefined
import random
import string
import sys
import yaml

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

jinja_env = Environment(
    loader=FileSystemLoader('.'),
    undefined = StrictUndefined
)

TPL_STREAMER_INSTRUCTIONS = jinja_env.get_template('streamer-setup.md.tpl')
TPL_NGINX_RTMP_SETTINGS = jinja_env.get_template('60-mod-rtmp.conf.tpl')
TPL_OBS_SETTINGS = jinja_env.from_string("{{ participant['name'] }}: rtmp://stream.pwny.racing/participant_{{ participant['id'] }}/{{ participant['password'] }}?psk={{ episode['password'] }}")

def get_episode_number():
    try:
        return int(sys.argv[1])
    except (IndexError, ValueError):
        print('Usage: %s <episode number>' % sys.argv[0])
        sys.exit(1)

def create_default_episode_config(episode_number, episode_cfg_path):
    episode_default_config = {
        'episode': {
            'number': episode_number,
            'password': generate_key(),
            'youtube_key': '',
        },
        'participants': [
            {'id': i, 'name': 'TBD', 'email': '', 'password': generate_key(), 'youtube_key': ''} for i in range(4)
        ]
    }
    with open(episode_cfg_path, 'w') as fout:
        yaml.safe_dump(episode_default_config, fout, default_flow_style=False, sort_keys=False)

def generate_episode_instructions(episode_cfg):
    for participant in episode_cfg['participants']:
        print(TPL_STREAMER_INSTRUCTIONS.render({'episode': episode_cfg['episode'], 'participant': participant}))

    for participant in episode_cfg['participants']:
        print(TPL_OBS_SETTINGS.render({'episode': episode_cfg['episode'], 'participant': participant}))

    print('-'*10)
    print('60-mod-rtmp.conf:')
    print(TPL_NGINX_RTMP_SETTINGS.render(episode_cfg))

if __name__ == '__main__':
    episode_number = get_episode_number()
    episode_cfg_path = 'episodes/%d.yaml' % episode_number

    try:
        with open(episode_cfg_path, 'r') as fin:
            episode_cfg = yaml.safe_load(fin)
    except FileNotFoundError:
        create_default_episode_config(episode_number, episode_cfg_path)
        print('Created default config for episode %d. Edit %s and re-run script' % (episode_number, episode_cfg_path))
        sys.exit(0)

    generate_episode_instructions(episode_cfg)
