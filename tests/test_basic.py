import pytest

from testsuite.databases import pgsql


# Start the tests via `make test-debug` or `make test-release`


async def test_first_time_users(service_client):
    response = await service_client.post(
        '/v1/hello',
        params={'name': 'userver'},
    )
    assert response.status == 200
    assert response.text == 'Hello, userver!\n'


async def test_db_updates(service_client):
    response = await service_client.post('/v1/hello', params={'name': 'World'})
    assert response.status == 200
    assert response.text == 'Hello, World!\n'

    response = await service_client.post('/v1/hello', params={'name': 'World'})
    assert response.status == 200
    assert response.text == 'Hi again, World!\n'

    response = await service_client.post('/v1/hello', params={'name': 'World'})
    assert response.status == 200
    assert response.text == 'Hi again, World!\n'

async def test_general(service_client):
    response = await service_client.post('/v1/user/sign_up', params={
        'email': 'dz2003@bk.ru',
        'passwd': 'qwertyuiop'
    })

    assert response.status == 201
    assert response.text == 'ok\n'

    response = await service_client.get('/v1/user/get_id', params={
        'email': 'dz2003@bk.ru'
    })

    assert response.status == 200
    assert response.text == '1\n'

    response = await service_client.post('/v1/user_data/add', params={
        'user_id': '1',
        'user_name': 'Dima',
        'surname': 'Zubkov',
        'gender': 'male',
        'date_of_birth': '2003-12-17',
        'user_status': 'online',
        'age': '19',
        'city': 'Moscow'
    })
    
    assert response.status == 201
    assert response.text == 'ok\n'

    response = await service_client.post('/v1/post/create', data='abboba', params={
        'user_id': '1',
    })

    assert response.status == 201
    assert response.text == 'ok\n'

    response = await service_client.post('/v1/user/change_passwd', params={
        'user_id': '1',
        'old_passwd': 'qwertyuiop',
        'new_passwd': 'asdfghjkl'
    })

    # Should be 200
    assert response.status == 201
    assert response.text == 'ok\n'

    response = await service_client.get('/v1/user/sign_in', params={
        'email': 'dz2003@bk.ru',
        'passwd': 'asdfghjkl'
    })

    assert response.status == 200
    assert response.text == '1'

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_db_initial_data(service_client):
    response = await service_client.post(
        '/v1/hello',
        params={'name': 'user-from-initial_data.sql'},
    )
    assert response.status == 200
    assert response.text == 'Hi again, user-from-initial_data.sql!\n'
