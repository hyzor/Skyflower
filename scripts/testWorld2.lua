player = 1 --player id


--Intro script--
----------------

function intro()
-- syntax: AddPoint(x, y, z, yaw, pitch, time(in seconds))
	AddPoint(0, 100, -100, 0, 45, 5)
	AddPoint(180, 150, -120, 0, 45, 5)
	AddPoint(130, 100, 100, 90, 20, 5)
	AddPoint(350, 120, 100, 0, 20, 5)
	AddPoint(500, 200, -50, 0, 45, 5)
end


--platform_start script--
-------------------------

function activated_platform_start(id)
	SetContinous(24, true)
end


buttontime = 0
inRange = false
function update_aiPush(id, dt)
	

	if not trapButtonDown then
		inRange = false
	
		SetTarget(id, 64) --start button
		if IsTouching(id, 64) then
			Jump(id)
		end
		
		if IsActivated(64) and IsStanding(id, 64) then --if button is down
			trapButtonDown = true
		end
	else
		SetTarget(id, player) --hunt player
		
		--if InRange(id, player, 30) then
			inRange = true --attack with both ai
			
			if CanPush(id, player) then
				Push(id, player)
			end
		--else
			--inRange = false
		--end
		
		--reactivate button after 5 sec
		buttontime = buttontime + dt
		if buttontime > 5 then
			buttontime = 0
			trapButtonDown = false
			inRange = false
		end
	end
	
function deactivated_platform_start(id)
	SetContinous(24, false)
end


--btnStair script--
-------------------

stairdown = true
function activated_btnStair(id)
	if stairdown then
		MoveToTarget(20)
		MoveToTarget(80)
		MoveToTarget(23)
		MoveToTarget(10)
		MoveToTarget(90)
		stairdown = false
	else
		MoveToSpawn(20)
		MoveToSpawn(80)
		MoveToSpawn(23)
		MoveToSpawn(10)
		MoveToSpawn(90)
		stairdown = true
	end
end



--aiPush script--
-----------------

function load_aiPush(id)
	Print("AI loaded")
	
	StartUpdate()
end

function update_aiPush(id, dt)
	if not stairdown then
		SetTarget(id,16)
	else
		SetTarget(id,player)
	end
	
end
