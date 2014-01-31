player = 1 --player id


--Intro script--
----------------

function intro()
-- syntax: AddPoint(x, y, z, yaw, pitch, time(in seconds))
	AddPoint(0, 100, -100, 0, 45, 1)
	AddPoint(180, 150, -120, 0, 45, 2)
	AddPoint(130, 100, 100, 90, 20, 2)
	AddPoint(350, 120, 100, 0, 20, 1)
	AddPoint(500, 200, -50, 0, 45, 1)
end


--platform_start script--
-------------------------

function activated_platform_start(id)
	SetContinous(24, true)
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

rush = true
rushtime = 0
scaredtime = 0
function update_aiPush(id, dt)
	if IsTouching(id, 12) or IsTouching(id, 13) then
		Jump(id)
	end

	if not stairdown then
		SetTarget(id,16)
		if IsActivated(16) and IsStanding(id, 16) then
			Jump(id)
		end
	else
		
		if rush then
			SetTarget(id, player)
			
			if InRange(id, player, 15) then
				SetSpeed(id, 50)
				if CanPush(id, player) then
					Push(id, player)
					rush = false
					scaredtime = 0
				elseif rushtime > 2 then
					rush = false
					scaredtime = 0
				end
			
				rushtime = rushtime + dt
			else
				SetSpeed(id, 10)
			end
		else
			SetTarget(id, player, 30)
			SetSpeed(id, 30)
			Push(player, id)
			if scaredtime > 4 then
				rush = true
				rushtime = 0
			end
			
			scaredtime = scaredtime + dt
		end
	end
	
end
