from flask import Flask, request, jsonify
import requests
import logging
application = Flask(__name__)

if __name__ != '__main__':
    gunicorn_logger = logging.getLogger('gunicorn.error')
    application.logger.handlers = gunicorn_logger.handlers
    application.logger.setLevel(gunicorn_logger.level)

PARTICIPANTS = {
    'laxa': 0,
    'mrtumble': 1,
    'kileak': 2,
    'Lord_Idiot': 3,
}

COMMUNITY_CHALLENGES = [2,6,4]
RACE_CHALLENGES = [1,3,5,7]

SLACK_URL = 'https://hooks.slack.com/services/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'
#SLACK_URL = 'https://hooks.slack.com/services/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' #Testing

# TODO: Not used at the moment
FLAGS = {
    1: 'pwny{congratulations-on-finishing-the-pwnable-race}',
    2: 'pwny{community_challenge_one_making_history}',
    3: 'pwny{second_time_around_the_effect_still_lasts}',
}

def get_unique_user(user):
    users_with_prefix = [name for name in PARTICIPANTS.keys() if name.lower().startswith(user.lower())]
    if len(users_with_prefix) == 1:
        return users_with_prefix[0]
    else:
        return False

def submit_flag(challenge, user):
    user_slot = PARTICIPANTS[user]
    try:
        return requests.post('http://localhost:9090/pwnyracing/flag', data={'userslot': user_slot, 'username': user, 'challenge': challenge}).json()['winner']
    except:
        return False

def create_result():
    return {
        'status': 'error',
        'message': 'Unknown error',
    }

def handle_race_challenge(challenge, user_token):
    result = create_result()
    user = get_unique_user(user_token)
    if not user:
        result['message'] = 'User not unique, please provide full username'
        return jsonify(result)
    
    if submit_flag(challenge, user):
        result['status'] = 'win'
        result['message'] = 'Congratulations! You have won the Pwny Race!'
    else:
        result['status'] = 'lose'
        result['message'] = 'Unfortunately someone else already submitted the flag. Better luck next time.'
    return jsonify(result)

def handle_community_challenge(challenge, user_token):
    result = create_result()
    user_token_clean = user_token.split('\n')[0].strip()
    application.logger.info('Solved community challenge: %d, E-mail: %s', challenge, user_token_clean)
    
    result['status'] = 'win'
    result['message'] = 'Congratulations! You have solved the community challenge. We will contact you with the results later.'

    slack_message = {
        'text': 'User with email `%s` solved the community challenge!' % user_token_clean
    }
    r = requests.post(SLACK_URL, json=slack_message)
    if r.status_code != 200:
        application.logger.error('Failed to post Slack message: %s', r.text)
    else:
        application.logger.info('Posted Slack message: %s', r.text)

    return jsonify(result)

@application.route('/challenges/<int:challenge>/flag')
def challenge_flag(challenge):
    user_token = request.args.get('token', False)
    if not user_token:
        result['message'] = 'Token not provided'
        return jsonify(result)

    if challenge in COMMUNITY_CHALLENGES:
        return handle_community_challenge(challenge, user_token)
    elif challenge in RACE_CHALLENGES:
        return handle_race_challenge(challenge, user_token)
    else:
        result = create_result()
        result['message'] = 'Challenge %d does not exist' % int(challenge)
        return jsonify(result)
