from flask import Flask, request, jsonify
import requests
application = Flask(__name__)

PARTICIPANTS = {
    'borysp': 0,
    'hpmv': 1,
    'vos': 2,
    'zap': 3,
}

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

@application.route('/challenges/<int:challenge>/flag')
def challenge_flag(challenge):
    result = {
        'status': 'error',
        'message': 'Unknown error',
    }

    user_token = request.args.get('token', False)
    if not user_token:
        result['message'] = 'Token not provided'
        return jsonify(result)

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

